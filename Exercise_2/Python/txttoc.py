from sys import stdin

#Converts exported output from audacity into C array

#Use:
#python txttotoc.py < sample-data-txt

c_file = open('wololo.c','w')


c_file.write("const uint16_t wololo[] = {\n")

print stdin.readline().strip()
print stdin.readline().strip()


test = stdin.readline().strip()
print test

c_file.write(str(test.split()[2]))
print stdin.readline().strip()
print stdin.readline().strip()
print stdin.readline().strip()
print stdin.readline().strip()

for line in stdin:
	tall = int((float(line.strip())+1)*(2**11))
	c_file.write(","+str(tall))
c_file.write("\n};")


c_file.close()

