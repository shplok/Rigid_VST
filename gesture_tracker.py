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
hands = mp_hands.Hands(max_num_hands=2, min_detection_confidence=0.7)

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

    if results.multi_hand_landmarks and results.multi_handedness:
        for idx, (hand_landmarks, handedness) in enumerate(zip(results.multi_hand_landmarks, results.multi_handedness)):
            mp_draw.draw_landmarks(img, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            gesture, direction, orientation = classify_gesture(hand_landmarks.landmark)
            label = handedness.classification[0].label[0]  # 'L' or 'R'

            x_offset = 10 if label == 'L' else 350 
            y_offset = 30

            cv2.putText(img, f'{label}: Gesture: {gesture}', (x_offset, y_offset),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.8, (65, 255, 0), 2)
            cv2.putText(img, f'{label}: Direction: {direction}', (x_offset, y_offset + 25),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 0), 2)
            cv2.putText(img, f'{label}: Orientation: {orientation}', (x_offset, y_offset + 45),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)

        # Send OSC only if gesture has changed
        if gesture != last_gesture:
            osc_client.send_message("/gesture", gesture)
            osc_client.send_message("/hand/direction", direction)
            osc_client.send_message("/hand/orientation", orientation)
            last_gesture = gesture

            # Send OSC only if gesture has changed
            if gesture != last_gesture:
                osc_client.send_message("/gesture", gesture)
                osc_client.send_message("/hand/direction", direction)
                osc_client.send_message("/hand/orientation", orientation)
                last_gesture = gesture

    cv2.imshow("Hand Tracker", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    if cv2.getWindowProperty("Hand Tracker", cv2.WND_PROP_VISIBLE) < 1:
        break

cap.release()
cv2.destroyAllWindows()
