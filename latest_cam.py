from time import sleep
import cv2 as cv
import numpy as np
from urllib.request import urlopen
face_cascade = cv.CascadeClassifier('haarcascade_frontalface_alt.xml')

url = "http://192.168.43.157:81/stream"
CAMERA_BUFFRER_SIZE = 1024

stream = urlopen(url)
bts = b''
i = 0
while True:
    try:
        bts += stream.read(CAMERA_BUFFRER_SIZE)
        jpghead = bts.find(b'\xff\xd8')
        jpgend = bts.find(b'\xff\xd9')
        if jpghead > -1 and jpgend > -1:
            jpg = bts[jpghead:jpgend+2]
            bts = bts[jpgend+2:]
            buff = np.frombuffer(
                jpg, dtype=np.uint8)
            if len(buff) > 0:
                img = cv.imdecode(buff, cv.IMREAD_UNCHANGED)
                img = cv.rotate(img, cv.ROTATE_90_COUNTERCLOCKWISE)
                img = cv.flip(img, 0)
                gray_img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
                detected_faces = face_cascade.detectMultiScale(
                    gray_img, 1.1, minNeighbors=5, minSize=(30, 30), flags=cv.CASCADE_SCALE_IMAGE)

                for (y, x, w, h) in detected_faces:
                    cv.rectangle(img, (y, x), (y+w, x+h), (250, 0, 0), 2)
                    cv.putText(img, 'FACE', (y, x), 1, 2, (255, 255, 255), 3)
                    roi_gray = gray_img[x:x+h, y:y+w]
                    roi_color = img[x:x+h, y:y+w]

                cv.imshow("a", img)
        k = cv.waitKey(1)
    except Exception as e:
        print("Error:" + str(e))
        bts = b''
        stream = urlopen(url)
        continue

    k = cv.waitKey(1)

    if k & 0xFF == ord('q'):
        break
cv.destroyAllWindows()
