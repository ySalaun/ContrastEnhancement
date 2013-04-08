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
for sdi in {0, 0.05, 0.1, 0.2}:
	for sd in {1,5,10,20,50,150}:
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
		for s in {-10,-5,-2,-0.5,0.5,2,5,10}:
			print 'sdi =', sdi, 'sd=', sd, 's=', s
			## OUTPUT PATH
			output = path + 'Images\\' + name + '\\sdi=' + str(sdi) + '\\s=' + str(s)
			## CREATE FOLDER IF NECESSARY
			if not os.path.exists(output): 
				os.makedirs(output)
			output += '\\' + name + '_s=' + str(s) + '_sd=' + str(sd) + '_sdi=' + str(sdi) + '.png'
			# compute usm
			os.system(path + 'Build\\bin\\Release\\main.exe' + ' ' + img + ' ' + filtered + ' ' + output + ' ' + str(sd) + ' ' + str(s) + ' ' + str(sdi) )