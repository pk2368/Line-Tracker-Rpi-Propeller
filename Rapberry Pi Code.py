# import the OpenCV library for computer vision
import cv2
import RPi.GPIO as GPIO
from gpiozero import Servo
from time import sleep
from picamera.array import PiRGBArray
from picamera import PiCamera

servo = Servo(23,minimum_pulse_width=1.05,maximum_pulse_width=2.95)

comm_in = 26
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(comm_in,GPIO.IN)

# Load the dictionary that was used to generate the markers.
# There's different aruco marker dictionaries, this code uses 6x6
dictionary = cv2.aruco.Dictionary_get(cv2.aruco.DICT_6X6_250)

# Initialize the detector parameters using default values
parameters = cv2.aruco.DetectorParameters_create()

parameters.minMarkerPerimeterRate = 0.5


# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
#camera.framerate = 32
#rawCapture = PiRGBArray(camera, size=(640, 480))
servo.min()
def servo_hit():
    servo.max()
    sleep(0.5)
    servo.min()
    sleep(0.5)

mrks_list = []
sleep(1)
while True:#Program keeps running forever
    try:
        # loop that runs the program forever
        #comm_status = GPIO.input(comm_in)
        if GPIO.input(comm_in):

            print("Running Marker Detection!!")
            # creates an "img" var that takes in a camera frame
            camera.capture("img.jpg")
            img = cv2.imread("img.jpg")

            # Convert to grayscale
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            # detect aruco tags within the frame
            markerCorners, markerIds, rejectedCandidates = cv2.aruco.detectMarkers(gray, dictionary, parameters=parameters)
            # draw box around aruco marker within camera frame
            #img = cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)
            # if a tag is found...
            if markerIds is not None:
                # for every tag in the array of detected tags..
                print(markerIds[0])
                detected_mkrs = int(markerIds[0])
                
                if detected_mkrs < 11:
                    print("Good guy!")
                    camera.capture("img_recog1.jpg")
                    markerIds = [None]
                    #run = int(input("Enter 1 to continue 0 to stop: "))
                else:
                    print("Bad Guys!!")
                    if (detected_mkrs in mrks_list):
                        markerIds[0] = 0
                        sleep(2)
                    else:
                        servo_hit()
                        mrks_list.append(detected_mkrs)
                        print(mrks_list)
                        camera.capture("img_recog2.jpg")
                        markerIds[0] = 0
                    #run = int(input("Enter 1 to continue 0 to stop: "))
                #GPIO.output(comm_out,1)
                sleep(3)
        else:
            print("Waiting for input")
            sleep(0.5)

    except KeyboardInterrupt:
        camera.close()
        cv2.destroyAllWindows()
        GPIO.cleanup()

# When everything done, release the capture
print("Program is Stopping!!")