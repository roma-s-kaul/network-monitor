import csv
import datetime 
inputFile = 'stageA.csv'
outputFile = inputFile.split('.', 1)[0] + '.tsv'
writeFileDesc = open(outputFile, 'w')
readFileDesc = open(inputFile, 'r')
serverIP = "10.1.3.3"

readCSV = csv.reader(readFileDesc, delimiter=',')
writeTSV = csv.writer(writeFileDesc, delimiter='\t')
#print(readCSV)
startTime = 0
txPacket = 0
txBytes = 0
rxPacket = 0
rxBytes = 0
oldOffset = 1
writeTSV.writerow(['time', 'txpkts', 'txbytes', 'rxpkts', 'rxbytes'])
for row in readCSV:
	#print(row)
	if not startTime:
		startTime = long(float(row[0]))

	currentOffset = (long(float(row[0])) - startTime) + 1


	if currentOffset != oldOffset:
		#dump stats
		#print(oldOffset, txPacket, txBytes, rxPacket, rxBytes)
		writeTSV.writerow([oldOffset, txPacket, txBytes, rxPacket, rxBytes])
		txPacket = 0
		txBytes = 0
		rxPacket = 0
		rxBytes = 0
		oldOffset = currentOffset
	
	#print('else-----', row[2])
	if serverIP == row[1]:
		txPacket += 1
		txBytes += long(row[2])
	else:
		rxPacket += 1
		rxBytes += long(row[2])
	

#print(oldOffset, txPacket, txBytes, rxPacket, rxBytes)
writeTSV.writerow([oldOffset, txPacket, txBytes, rxPacket, rxBytes])

writeFileDesc.close()
readFileDesc.close()


        