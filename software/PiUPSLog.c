/* PiUPS Log
 *
 * Simple application to log infomation from PiUPS to a file
 *
 * 
 */

#include <unistd.h>                             //Needed for I2C port
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "i2ctools.h"
#include "PiUPSDefines.h"
#include "PiUPStools.h"

const uint8_t ouput_num = 8;
const char* output_names[] = {"VCC",      "VBatt",    "VAux1",     "VAux2",     "VRail",    "IRail",      "5VO",     "VAUXO",};
const uint16_t output_regs[] = {PiUPSVcc, PiUPSVBatt, PIUPS_VAUX1, PIUPS_VAUX2, PIUPS_VRAIL, PIUPS_IRAIL, PIUPS_V5V, PIUPS_VAUXO};

const uint16_t max_retries = 20;
const uint32_t sleep_time = 5000;

// Read with a retry function:
int read_retry(int file_i2c, uint8_t reg, uint16_t* result);

static volatile int go = 1;
void exitHandler(int dummy) {
    go = 0;
}



int main (int argc, char* argv[])
{
  // I2C config
  int file_i2c;
  char *filename_i2c = (char*)"/dev/i2c-1";
  
  // Output config
  FILE * file_log;
  char *filename_log = (char*)"test.log";
  time_t time_now;
  struct tm * timeinfo_now;
  char time_str[80];
  
  // Parameters:
  uint16_t read_vals[ouput_num];
  int read_failures = 0;
  
  // Iteration variables.
  int i;
  
  //=======================================================================

  // Open the I2C device:
  if (open_i2c_dev(&file_i2c, filename_i2c, 0x13) != 0)
  {
    printf("Failed to setup i2c");
    return(-1);
  }
  
  // Open the log file:
  file_log = fopen(filename_log, "w");
  if (file_log == NULL)
  {
    printf("Failed to open output file");
	return(-1);  
  }
  
  fprintf(file_log, "Date Time, UnixTime");
  for (i=0; i<ouput_num; i++) fprintf(file_log, ", %s",  output_names[i]);
  fprintf(file_log, "\n");
  
  signal(SIGINT, exitHandler);
  // Read the data:
  while(go)
  {
    // Read all the vaulues over i2c, in the event of a persistent
	// failure increment read_failures and stop further attempts to
	// read registers.
	read_failures = 0;
	time(&time_now);
	timeinfo_now = localtime(&time_now);
	for (i=0; i<ouput_num; i++)
	{ 
      if (read_retry(file_i2c, output_regs[i], &read_vals[i]) !=0)
	  {
	    read_failures++;
		break;
	  }
	  usleep(sleep_time);
    }
	
	// If all data was read correctly write the infomation to a fiu
	if (read_failures == 0)
    {
	  strftime (time_str,80,"%F %T",timeinfo_now);
	  fprintf(file_log, "%s, %lu", time_str, (unsigned long) time_now );
	  for (int i=0; i<ouput_num; i++) fprintf(file_log, ", %d",  read_vals[i]);
	  fprintf(file_log, "\n");
	}
	sleep(10);
  }
  
	
}

int read_retry(int file_i2c, uint8_t reg, uint16_t* result)
{
  for (int i=max_retries; i>0; i--)
  {
	if (read_i2c_reg(file_i2c, reg, result) == 0) return 0;
	usleep(sleep_time);
  }
  return 1;
}