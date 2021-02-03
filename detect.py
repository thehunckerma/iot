import cv2 as cv

face_cascade = cv.CascadeClassifier('haarcascade_frontalface_alt.xml')
body_cascade = cv.CascadeClassifier('haarcascade_fullbody.xml')
upper_cascade = cv.CascadeClassifier('haarcascade_upperbody.xml')
lower_cascade = cv.CascadeClassifier('haarcascade_lowerbody.xml')

# real-time capture
capture = cv.VideoCapture(0)
while True:
    ret, frame = capture.read()

    if not ret:
        print("VideoCapture read no frame")
        break
    gray_img = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

    detected_faces = face_cascade.detectMultiScale(
        gray_img, 1.1, minNeighbors=5, minSize=(30, 30), flags=cv.CASCADE_SCALE_IMAGE)
    for (y, x, w, h) in detected_faces:
        cv.rectangle(frame, (y, x), (y+w, x+h), (250, 0, 0), 2)
        cv.putText(frame, 'FACE', (y, x), 1, 2, (255, 255, 255), 3)
        roi_gray = gray_img[x:x+h, y:y+w]
        roi_color = frame[x:x+h, y:y+w]

    detected_upper = upper_cascade.detectMultiScale(
        gray_img, 1.09, minNeighbors=5, flags=cv.CASCADE_SCALE_IMAGE)
    for (uy, ux, uw, uh) in detected_upper:
        cv.rectangle(frame, (uy, ux), (uy+uw, ux+uh), (0, 250, 0), 2)
        cv.putText(frame, 'UPPER_BODY', (uy, ux), 1, 2, (255, 255, 255), 2)

    detected_body = body_cascade.detectMultiScale(
        gray_img, 1.09, minNeighbors=5, flags=cv.CASCADE_SCALE_IMAGE)
    for (uy, ux, uw, uh) in detected_body:
        cv.rectangle(frame, (uy, ux), (uy+uw, ux+uh), (0, 0, 250), 2)
        cv.putText(frame, 'BODY', (uy, ux), 1, 2, (255, 255, 255), 2)

    detected_lower = lower_cascade.detectMultiScale(
        gray_img, 1.09, minNeighbors=5, flags=cv.CASCADE_SCALE_IMAGE)
    for (uy, ux, uw, uh) in detected_lower:
        cv.rectangle(frame, (uy, ux), (uy+uw, ux+uh), (70, 50, 130), 2)
        cv.putText(frame, 'LOWER_BODY', (uy, ux), 1, 2, (255, 255, 255), 2)

    cv.imshow('Detect', frame)

    if cv.waitKey(1) & 0xFF == ord('q'):
        break

# cv.imshow('img',img)
capture.release()
cv.destroyAllWindows()
