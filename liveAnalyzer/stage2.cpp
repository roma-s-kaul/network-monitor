#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sstream>

using namespace std;

string execCommand(string command) {
   char buffer[128];
   string commandResult = "";

   //Open the pipe
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }

   //Read till EoP (End of Process):
   while (!feof(pipe)) {

      //Use buffer to read and add to commandResult
      if (fgets(buffer, 128, pipe) != NULL)
         commandResult += buffer;
   }

   //Close the pipe
   pclose(pipe);
   return commandResult;
}

long long convertStringToInt(string input){ 
   input.replace(input.end() - 1, input.end(), "\0");
   stringstream inputStr(input);
   long long newInteger = 0;
   inputStr >> newInteger;
   return newInteger;
}

int main() {
    //command to get the list of available network interfaces and their corresponding inet addresses
   string interf = execCommand("/sbin/ip -4 -o a | cut -d ' ' -f 2,7 | cut -d '/' -f 1");

   //get the index of the string interf corresponding to server ip address - 10.1.3.3
   int found = interf.find("10.1.3.3");

   //network interface
   string prefix = "eth";
   string interface = prefix + interf[(found - 2)];

   //statistics fetching
   int counter = 0;
   int lastnp = 0;
   int lastep = 0;
   int lastfp = 0;
   int lasttp = 0;
   long long oldTxPackets = 0;
   long long newTxPackets = 0;
   long long oldRxPackets = 0;
   long long newRxPackets = 0;
   long long oldTxBytes = 0;
   long long newTxBytes = 0;
   long long oldRxBytes = 0;
   long long newRxBytes = 0;
   long long diffTxPackets = 0;
   long long diffRxPackets = 0;
   long long diffTxBytes = 0;
   long long diffRxBytes = 0;
   
   FILE* dumpOutput = fopen("stage_2_network_exp2a.tsv", "w");
   fprintf(dumpOutput, "time\ttxpkts\ttxbytes\trxpkts\trxbytes\n");

   FILE* dumpOutput2 = fopen("stage_2_events_exp2b.tsv", "w");
   fprintf(dumpOutput2, "time\tevent\n");

   FILE* dumpOutput3 = fopen("stage_2_events_exp2c.tsv", "w");
   fprintf(dumpOutput3, "time\tevent\n");	
   while(1) {
      string txpackets = execCommand("cat /sys/class/net/"+interface+"/statistics/tx_packets");
      string txbytes = execCommand("cat /sys/class/net/"+interface+"/statistics/tx_bytes");
      string rxpackets = execCommand("cat /sys/class/net/"+interface+"/statistics/rx_packets");
      string rxbytes = execCommand("cat /sys/class/net/"+interface+"/statistics/rx_bytes");

      txpackets.replace((txpackets.length() - 1), 1, 1, '\0');
      newTxPackets = convertStringToInt(txpackets);
      diffTxPackets = newTxPackets - oldTxPackets;
      oldTxPackets = newTxPackets;

      rxpackets.replace(rxpackets.length() - 1, 1, 1, '\0');
      newRxPackets = convertStringToInt(rxpackets);
      diffRxPackets = newRxPackets - oldRxPackets;
      oldRxPackets = newRxPackets;

      txbytes.replace((txbytes.length() - 1), 1, 1, '\0');
      newTxBytes = convertStringToInt(txbytes);
      diffTxBytes = newTxBytes - oldTxBytes;
      oldTxBytes = newTxBytes;

      rxbytes.replace(rxbytes.length() - 1, 1, 1, '\0');
      newRxBytes = convertStringToInt(rxbytes);
      diffRxBytes = newRxBytes - oldRxBytes;
      oldRxBytes = newRxBytes;
      counter += 1;

      char outputLog[500];
      sprintf(outputLog, "%d\t%lli\t%lli\t%lli\t%lli", counter-1, diffTxPackets, diffTxBytes, diffRxPackets, diffRxBytes);
      if(counter > 1){
         //cout<<outputLog<<"\n";
		 fprintf(dumpOutput, "%s\n", outputLog);
         fflush (dumpOutput);
         //cout<<((diffTxBytes + diffRxBytes)/125000)<<endl;
         if((diffTxBytes + diffRxBytes)/125000 >= 90) {
            if(lastnp < counter - 2) {
				cout<<"90% triggered\t"<<counter-1<<endl;
                fprintf(dumpOutput2, "%d\t>=90%%\n", (counter-1));
                fprintf(dumpOutput3, "%d\t>=90%%\n", (counter-1));
			}
            fflush (dumpOutput2);
 			fflush (dumpOutput3);
			lastnp = counter - 1;
         } else if((diffTxBytes + diffRxBytes)/125000 >= 80) {
            if(lastep < (counter - 2)) {
				cout<<"80% triggered\t"<<counter-1<<endl;
            	fprintf(dumpOutput2, "%d\t>=80%%\n", counter-1);
			    fprintf(dumpOutput3, "%d\t>=80%%\n", counter-1);
			}
            fflush (dumpOutput2);
			fflush (dumpOutput3);
            lastep = counter - 1;
         } else if((diffTxBytes + diffRxBytes)/125000 >= 50) {
			if(lastfp < (counter - 2)){
				cout<<"50% triggered\t"<<counter-1<<endl;
            	fprintf(dumpOutput2, "%d\t>=50%%\n", counter-1);
				fprintf(dumpOutput3, "%d\t>=50%%\n", counter-1);
			}
            fflush (dumpOutput2);
			fflush (dumpOutput3);
            lastfp = counter - 1;
        } else {
			if(lasttp < (counter - 2)) {
				cout<<"<50% triggered\t"<<counter-1<<endl;
				fprintf(dumpOutput2, "%d\t<50%%\n", counter-1);
				fprintf(dumpOutput3, "%d\t<50%%\n", counter-1);
			}
			fflush (dumpOutput2);
			fflush (dumpOutput3);
            lasttp = counter - 1;
		}
      }
      sleep(1);
  }
  fclose(dumpOutput);
  fclose(dumpOutput2);
  fclose(dumpOutput3);
  return 0;
}

