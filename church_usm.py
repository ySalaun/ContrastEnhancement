import os

path = 'C:\\Users\\Yohann\\Documents\\GitHub\\ContrastEnhancement\\'

## FILE NAME
name = 'church'
img = path + 'Images\\' + name + '\\' + name + '.png'

## LOOP ON PARAMETERS
method = '0'

for sd in {1,5,10,20,50,150}:
	for s in {-10,-5,-2,-0.5,0.0,0.5,2,5,10}:
		## OUTPUT PATH
		output = path + 'Images\\' + name + '\\' + name + '_method=' + str(method) + '_s=' + str(s) + '_sd=' + str(sd) + '.png'
		os.system('C:\\Users\\Yohann\\Documents\\GitHub\\ContrastEnhancement\\Build\\bin\\Release\\main.exe' + ' ' + img + ' ' + str(method) + ' ' + output + ' ' + str(sd) + ' ' + str(s) )