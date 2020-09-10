import cv2
import numpy as np

# Global variables
api = 0
kernel = np.ones((5,5),np.uint8)		# kernel untuk keperluan morfologi 'closing' gambar
thres_lower = np.array([0,5,200])		# batas minimum range nilai HSV dari api
thres_upper = np.array([10,255,255])	# batas maksimum range nilai HSV dari api

cam = cv2.VideoCapture(1)				# source kamera yang dipakai
cam.set(3,480)							# set ukuran lebar frame menjadi 480 pixel
cam.set(4,360)							# set ukuran tinggi frame menjadi 360 pixel

try:
	while cam.isOpened():				# loop
		ret, frame = cam.read()			# ambil gambar
		labeled_img = frame.copy()		# copy gambar untuk pelabelan
		
		# Preprocessing
		blur = cv2.GaussianBlur(frame, (5,5), 0)	# blurring untuk menghilangkan flicker	
		hsv = cv2.cvtColor(blur, cv2.COLOR_BGR2HSV)	# konversi colorspace menjadi HSV

		# Proses deteksi
		masking = cv2.inRange(hsv, thres_lower, thres_upper)	# masking untuk menyeleksi HSV dari api
		closing = cv2.morphologyEx(masking, cv2.MORPH_CLOSE, kernel)	# morfologi closing		
		contours, h = cv2.findContours(closing,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)	# deteksi kontur dari objek yang dianggap sbg api
		
		# seleksi object, jika luas kontur di atas 500, maka objek tersebut adalah api
		api = 0
		for c in contours:
			area = cv2.contourArea(c)
			if area > 500:
				api += 1
				x,y,w,h = cv2.boundingRect(c)
				cv2.rectangle(labeled_img,(x,y),(x+w,y+h),(0,255,0),2)
		
		cv2.imshow('display', labeled_img)
		
		# Hilangkan tanda pagar pada 'if' dibawah ini
		# if api != 0:
			# Isi program jika api terdeteksi
		# else:
			# Isi program jika api tidak terdeteksi

		if cv2.waitKey(1)&0xff == 27:
			break
	
	cam.release()
	cv2.destroyAllWindows()
	print('Program dihentikan')

except KeyboardInterrupt:
	cam.release()
	cv2.destroyAllWindows()
	print('Program dihentikan')
