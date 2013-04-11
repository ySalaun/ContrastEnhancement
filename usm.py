import sys
import os

path = 'C:\\Users\\Yohann\\Documents\\GitHub\\ContrastEnhancement\\'

## FILE NAME
name = sys.argv[1]
img = path + 'Images\\' + name + '\\' + name + '.png'
img_ppm = path + 'Images\\' + name + '\\' + name + '.ppm'
temp = path + 'Images\\' + name + '\\temp_' + name + '.ppm'

## Bilat path
bilat = path +'Bilat\\truncated_kernel_bf.exe'

## LOOP ON PARAMETERS
# bilateral filter params 
#	- sdi	(standard deviation for intensity sdi = 0 ==> gaussian)
#	- sd	(standard deviation for space sd = 0 ==> gaussian)
for sdi in {0}:				#for sdi in {0, 0.05, 0.1, 0.2}:
	for sd in {5,10,20}:	#	for sd in {1,5,10,20,50,150}:
		if sdi != 0:
			# create filtered picture path
			filtered = path + 'Images\\' + name + '\\sdi=' + str(sdi) + '\\'
			# create folder if neccessary
			if not os.path.exists(filtered): 
				os.makedirs(filtered)
			# filter the picture
			os.system(bilat + ' ' + img_ppm + ' ' + temp + ' ' + str(sd) + ' ' + str(sdi))
			filtered += 'sd=' + str(sd) + '_bilat.png'
			# convert ppm into png
			os.system('convert '+ temp + ' ' + filtered)
		else:
			filtered = img
		for l in {0.2,0.3,0.5, 0.99, 1, 2, 10}:
			for s in {0,0.5,1,2}:
				print 'sdi =', sdi, 'sd=', sd, 'l=', l, 's=', s
				## OUTPUT PATH
				output = path + 'Images\\' + name + '\\sdi=' + str(sdi) + '\\l=' + str(l) + '\\s=' + str(s)
				## CREATE FOLDER IF NECESSARY
				if not os.path.exists(output): 
					os.makedirs(output)
				output += '\\' + name + '_s=' + str(s) + '_l=' + str(l) + '_sd=' + str(sd) + '_sdi=' + str(sdi) + '.png'
				# compute usm
				os.system(path + 'Build\\bin\\Release\\main.exe' + ' ' + img + ' ' + filtered + ' ' + output + ' ' + str(sd) + ' ' + str(s) + ' ' + str(sdi)  + ' ' + str(l))