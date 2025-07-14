import cv2
import mediapipe as mp
from pythonosc.udp_client import SimpleUDPClient

# ========== OSC Config ==========
OSC_IP = "127.0.0.1"      # local plugin or receiver
OSC_PORT = 8000           # adjust this in your plugin too
osc_client = SimpleUDPClient(OSC_IP, OSC_PORT)

# ========== MediaPipe Setup ==========
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils
hands = mp_hands.Hands(max_num_hands=1, min_detection_confidence=0.7)

# ========== Gesture Logic ==========
def classify_gesture(landmarks):
    thumb_tip = landmarks[4].y
    index_tip = landmarks[8].y
    pinky_tip = landmarks[20].y
    wrist = landmarks[0].y

    fingers_up = sum([
        thumb_tip < wrist,
        index_tip < wrist,
        pinky_tip < wrist
    ])
    
    if fingers_up == 0:
        return "fist"
    elif fingers_up == 1:
        return "pointer"
    elif fingers_up == 3:
        return "palm"
    else:
        return "other"

# ========== Main Loop ==========
cap = cv2.VideoCapture(0)

while cap.isOpened():
    success, img = cap.read()
    if not success:
        continue

    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    results = hands.process(img_rgb)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(img, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            gesture = classify_gesture(hand_landmarks.landmark)
            cv2.putText(img, f'Gesture: {gesture}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

            # Send over OSC
            osc_client.send_message("/gesture", gesture)

    cv2.imshow("Hand Tracker", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
