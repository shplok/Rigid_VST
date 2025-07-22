import cv2
import mediapipe as mp
from pythonosc.udp_client import SimpleUDPClient

# ========== OSC Config ==========
OSC_IP = "127.0.0.1"
OSC_PORT = 8000
osc_client = SimpleUDPClient(OSC_IP, OSC_PORT)

# ========== MediaPipe Setup ==========
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils
hands = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.7)

# ========== Gesture Helpers ==========
def get_finger_curls(landmarks):
    return {
        'thumb': landmarks[4].x < landmarks[3].x,
        'index': landmarks[8].y > landmarks[6].y,
        'middle': landmarks[12].y > landmarks[10].y,
        'ring': landmarks[16].y > landmarks[14].y,
        'pinky': landmarks[20].y > landmarks[18].y,
    }

def get_pointing_direction(landmarks):
    wrist = landmarks[0]
    index_tip = landmarks[8]
    if abs(index_tip.x - wrist.x) > 0.2:
        return "left" if index_tip.x < wrist.x else "right"
    return "center"

def get_hand_orientation(landmarks):
    index_base = landmarks[5]
    pinky_base = landmarks[17]
    dx = pinky_base.x - index_base.x
    dy = pinky_base.y - index_base.y
    angle = cv2.fastAtan2(dy, dx)
    if angle < 45:
        return "right"
    elif angle > 135:
        return "left"
    else:
        return "upright"

def classify_gesture(landmarks):
    curls = get_finger_curls(landmarks)
    direction = get_pointing_direction(landmarks)
    orientation = get_hand_orientation(landmarks)

    if all(curls.values()):
        return "fist", direction, orientation

    if curls['thumb'] and not curls['index'] and all(curls[f] for f in ['middle', 'ring', 'pinky']):
        return f"pointing_{direction}", direction, orientation

    if not any(curls.values()):
        return "open_palm", direction, orientation

    if not curls['thumb'] and all(curls[f] for f in ['index', 'middle', 'ring', 'pinky']):
        return "thumbs_up", direction, orientation

    return "unknown", direction, orientation

# ========== Main Loop ==========
cap = cv2.VideoCapture(0)
last_gesture = ""

while cap.isOpened():
    success, img = cap.read()
    if not success:
        continue

    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    results = hands.process(img_rgb)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(img, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            gesture, direction, orientation = classify_gesture(hand_landmarks.landmark)
            cv2.putText(img, f'Gesture: {gesture}', (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            cv2.putText(img, f'Direction: {direction}', (10, 60),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 0, 0), 2)
            cv2.putText(img, f'Orientation: {orientation}', (10, 90),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)

            # Send OSC only if gesture has changed
            if gesture != last_gesture:
                osc_client.send_message("/gesture", gesture)
                osc_client.send_message("/hand/direction", direction)
                osc_client.send_message("/hand/orientation", orientation)
                last_gesture = gesture

    cv2.imshow("Hand Tracker", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
