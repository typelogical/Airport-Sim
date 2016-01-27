/********************************************************************************
Simulate the arrivals and departures at an airport in a time sequence of a 24-hour period. 
The airport has 2 runways. The simulation works in time units of one minute. During any one 
minute, planes may enter the system. The planes that enter the system may land or take-off 
depending on the input. The program must keep track of planes, assign planes to their 
respective runways, execute the take-offs and landing and keep track of the status of each plane.


The input for the program must be read from a file. The first line of the input (input.txt) 
contains the starting time which is specified as two integers as the initial hour and minute 
( 12 00 ). Each subsequent line specifies the increment of time, a request for a plane to enter 
the take-off queue, a request for a landing queue and a request for the status of a plane.

The request to enter a take-off queue :-

T MH 1000 12 15 200

T indicates this is a take off. The string MH 1000 specifies the airline and flight number. 
The integers 12 15 indicates the flight was supposed to take-off at 12:15 (pm). The plane 
should be immediately placed into the runway queue, even if the time is before the specified 
departure time. The number 200 indicates the number of passengers on the plane.

The request to land:-

L TW 1299 12 15 16 18 200

L indicates this is a landing request. The string TW 1299 and 12 15 are the same as the 
take-off request. The number 16 indicates the number of minutes until this plane will arrive 
at the airport and will be ready to land. The plane will not be assigned to a landing queue 
until 16 minutes later. The number 18 indicates the number of time units of fuel the plane 
has left. Assume that this number decrements along with the number of time units until the 
plane arrives at the airport. The integer 200 indicates the number of passengers.


Status check request:- 

S TW 1299


Assign each of the planes to a runway, choosing the runway with the fewest planes waiting to 
use it in each case. Break ties by choosing the runway with the lower number. Print the 
contents of the queue every minute.

Eg:-

Start of time 12:00

Runway 0
Take-off queue:
MH 1111 in queue for 1 minute
TW 1234 in queue for 1 minute
Landing queue:
No planes in queue.

Runway 1
Take-off queue:
AK 100 in queue for 1 minute
Landing queue:
PH 999 in queue for 1 minute
1 minute of fuel remain


*Look for emergency landings. An emergency landing is required when a plane 
has 0 units of fuel remaining. All runways are cleared and no other runway 
may be used during this minute.*

An output for an emergency landing:-

MH 1000 with 200 passengers successfully made an emergency landing on runway 0 at 15:12.
Expected time of arrival was 14:56.


Output for planes landing and taking off:-

MH 1231 with 200 passengers landed on runway 0 at 13:11
Expected arrival time was 13:01

AK 111 with 120 passengers departed on runway 1 at 13:11
Expected departure time was 13:01


The output statements should be printed on screen and into a self-defined file.

***********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#define TESTS 1
#undef TESTS
typedef struct Airplane {
    char airline[2];
    int id;
    int state;
    int takeOffTime;
    int timeUntilLanding;
    int numPassengers;
    int fuel;
    int timeInQeue;
} Airplane;

typedef struct QeueItem {
    Airplane airplane;
    struct QeueItem* nextItem;
} QeueItem;

typedef struct Runway {
    int planesQeued;
    int id;
    // 0 is takeoff qeue and 1 is landing qeue, 2 is the special qeue for
    // planes that will hold planes that are not ready to land
    QeueItem* qeue [3];
} Runway;

typedef struct Airport {
    int idPool[1000];
    int size;
    int runway;
    int qeueId;
    int time;
} Airport;
#define NUM_RUNWAYS 2
//Runway runways [NUM_RUNWAYS];
/*********************************************************************************
IO
 ********************************************************************************/
/* Globals */

Airport airport;
Runway* runways;
Airplane* airplanes;
int airportTime = 0;
FILE* in;
FILE* out;
/********************************************************************************/
void printAirplane (Airplane a) {
       if (a.state == 'T') {
           int hr = a.takeOffTime / 60, min = a.takeOffTime % 60;
            printf ("%c %s %i %i %i %i\n", a.state, a.airline, a.id, hr, min, a.numPassengers);
            fprintf (out, "%c %s %i %i %i %i\n", a.state, a.airline, a.id, hr, min, a.numPassengers);
       } else if (a.state == 'L') {
           int hr = a.takeOffTime / 60, min = a.takeOffTime % 60;
            printf ("%c %s %i %i %i %i %i %i\n", a.state, a.airline, a.id, hr, min, 
            a.timeUntilLanding, a.fuel, a.numPassengers);
            fprintf (out, "%c %s %i %i %i %i %i %i\n", a.state, a.airline, a.id, hr, min, 
            a.timeUntilLanding, a.fuel, a.numPassengers);
       } else if (a.state == 'S') {
           a.state = 'S';
           printf ("%c %s %i\n", a.state, a.airline, a.id);
           fprintf (out, "%c %s %i\n", a.state, a.airline, a.id);
       } else {
           printf ("Airplane struct is not valid\n");
           fprintf (out, "Airplane struct is not valid\n");
       }
}
int  readFile (FILE *in, Airplane airplanes[]) {
    if (in != NULL) {
        char flag;
        // Read file and parse the command
        int i = 0;
        while (! feof (in)) {
            Airplane a;
            //memset ( (void) &a, 0, sizeof (a));
           flag = getc (in);
           // parse
           int hr, min;
           if (flag == 'T') {
                fscanf (in, "%s %i %i %i %i", a.airline, &a.id, &hr, &min, &a.numPassengers);
                a.state = 'T';
                a.takeOffTime = 60 * hr + min;
                a.timeInQeue = 0;
                airplanes [i++] = a;
           } else if (flag == 'L') {
                fscanf (in, "%s %i %i %i %i %i %i", a.airline, &a.id, &hr, &min, &a.timeUntilLanding,
                &a.fuel, &a.numPassengers);
                a.state = 'L';       
                a.takeOffTime = 60 * hr + min;
                a.timeInQeue = 0;
                airplanes [i++] = a;
           } else if (flag == 'S') {
               a.state = 'S';
				a.timeInQeue = 0;
               fscanf (in, "%s %i", a.airline, &a.id);
               airplanes [i++] = a;
           } else {
               //printf ("Parse error.\n");
           }
          // printAirplane(a);
          
     
        }
        return i;
    }
    return -1;
}
void usage (char exe[]) {
	printf ("%s inFfile outFile\n", exe);
}
#define TAKEOFF_QEUE 0
#define LANDING_QEUE 1

/*********************************************************************************
Airport
 * *******************************************************************************/


/* Initialize glogal variables to 0 */
void init () {
	//memset (airport, 0, sizeof (airport));
	airplanes = malloc(sizeof(Airplane) * 100);
 	runways = malloc(sizeof (Runway) * NUM_RUNWAYS);
 	memset (airplanes, 0, sizeof (Airplane) * 100);
	memset (runways, 0, sizeof (Runway) * NUM_RUNWAYS);
	int i;
	for (i = 0; i < NUM_RUNWAYS; ++i) {
        runways [i].qeue [0] = NULL;
        runways [i].qeue [1]= NULL;
        runways [i].qeue [2]= NULL;
    }
	
}
//QeueItem* qeue = NULL;
void push (Airport airport, Runway runways[], Airplane airplane) {
    runways [airport.runway].qeue [airport.qeueId];
    if (runways [airport.runway].qeue [airport.qeueId] == NULL) {
        runways [airport.runway].qeue [airport.qeueId] = malloc (sizeof (QeueItem));
        runways [airport.runway].qeue [airport.qeueId]->airplane = airplane;
        runways [airport.runway].qeue [airport.qeueId]->nextItem = NULL;
        return;
    }
    QeueItem* currItem = runways [airport.runway].qeue [airport.qeueId];
   // printf("Printing...\n");
    while (currItem->nextItem != NULL) {
         //printf("currItem: %i\n", currItem);
        currItem = currItem->nextItem;
        //printf("qeue: %i\n", qeue);
    }
    QeueItem* nextItem = malloc (sizeof (QeueItem));
    nextItem->airplane = airplane;
    nextItem->nextItem = NULL;
    currItem->nextItem = nextItem;
}
QeueItem head;
QeueItem* pop (Airport airport, Runway runways[]) {
    //QeueItem* qeue = runways [airport.runway].qeue [airport.qeueId];

    if (runways [airport.runway].qeue [airport.qeueId] != NULL) {
        head= *runways [airport.runway].qeue [airport.qeueId];
        runways [airport.runway].qeue [airport.qeueId] = runways [airport.runway].qeue [airport.qeueId]->nextItem;
        runways [airport.runway].planesQeued -= 1;
        return &head;
    }
    return NULL;
}

void qeueTakeOff (Airplane airplane) {
	airport.qeueId = TAKEOFF_QEUE;
    if (runways[0].planesQeued <= runways [1].planesQeued) {
    	airport.runway = 0;
    } else {
    	airport.runway = 1;
    }
    runways [airport.runway].planesQeued += 1;
    push (airport, runways, airplane);
}
void qeueLanding (Airplane airplane) {
	airport.qeueId = LANDING_QEUE;
    if (runways[0].planesQeued <= runways [1].planesQeued) {
    	airport.runway = 0;
    } else {
    	airport.runway = 1;
    }
    runways [airport.runway].planesQeued += 1;
    push (airport, runways, airplane);
}

Airplane* findAirplane (Airplane airplane) {
	int i = 0;
	QeueItem* item;
    for (i; i < NUM_RUNWAYS; ++i) {
        item = runways [i].qeue [TAKEOFF_QEUE];
		// Search through takeoff qeue
		while (item != NULL) {
        	if (item->airplane.id == airplane.id) {
        		Airplane* airplanePtr = malloc (sizeof (Airplane));
        		*airplanePtr = item->airplane;
        		return airplanePtr;
			}
			item = item->nextItem;
	    }
		
		item = runways [i].qeue [LANDING_QEUE];
		// Search through landing qeue
		while (item != NULL) {
        	if (item->airplane.id == airplane.id) {
        		Airplane* airplanePtr = malloc (sizeof (Airplane));
        		*airplanePtr = item->airplane;
        		return airplanePtr;
			}
			item = item->nextItem;
		}
		
    }

 	item =runways[0].qeue [2];
    while (item != NULL) {
    	if (item->airplane.id == airplane.id) {
    		Airplane* airplanePtr = malloc (sizeof (Airplane));
    		*airplanePtr = item->airplane;
    		return airplanePtr;
		}
		item = item->nextItem;
	}
    return NULL;
}
void printAirplaneStatus (Airplane airplane) {
    Airplane* airplanePtr = findAirplane (airplane);
    if (airplanePtr != NULL) {
    	printf ("Status request: ");
    	fprintf (out, "Status request: ");
    	printAirplane (*airplanePtr);
	} else {
		printf ("Could not priint airplane status.\n Airplane not in qeue.\n");
	}
}
void printRunwayStatus (Runway runways[]) {
	int i;
	printf ("************************Runway Status**********************\n");
	fprintf (out, "************************Runway Status**********************\n");

	for (i = 0; i < NUM_RUNWAYS; ++i) {
		QeueItem* item = runways [i].qeue [TAKEOFF_QEUE];
		printf("Runway-%i------------------------------\n",i);
		printf ("Takeoff Qeue: \n");
		fprintf(out, "Runway-%i------------------------------\n",i);
		fprintf (out, "Takeoff Qeue: \n");
		if (item == NULL) {
			printf ("\tNo planes in qeue.\n");
			fprintf (out, "\tNo planes in qeue.\n");
		}
		while (item != NULL) {
			Airplane airplane = item->airplane;
			printf ("\t%s %i in qeue for %i minute\n", airplane.airline,
				airplane.id, airplane.timeInQeue);
			fprintf (out, "\t%s %i in qeue for %i minute\n", airplane.airline,
				airplane.id, airplane.timeInQeue);
			item = item->nextItem;
		}
		printf ("\nLanding Qeue: \n");
		fprintf (out, "\nLanding Qeue: \n");
		item = runways [i].qeue [LANDING_QEUE];
		if (item == NULL) {
			printf ("\tNo planes in qeue.\n");
			fprintf (out, "\tNo planes in qeue.\n");
		}
		while (item != NULL) {
			Airplane airplane = item->airplane;
			printf ("\t%s %i in qeue for %i minute\n", airplane.airline,
				airplane.id, airplane.timeInQeue);
			fprintf (out, "%s %i in qeue for %i minute\n", airplane.airline,
				airplane.id, airplane.timeInQeue);
			printf ("\t% minute of fuel remaining\n", airplane.fuel);
			fprintf (out, "%i minute of fuel remaining\n", airplane.fuel);
			item = item->nextItem;
		}
	}
	
}
void qeuePreLanding (Airplane airplane) {
	airport.runway = 0;
	airport.qeueId = 2;
	push (airport, runways, airplane);
}
void qeueFlight (Airplane airplane) {
    if (airplane.state == 'T') {
        qeueTakeOff (airplane);
    } else if (airplane.state == 'L') {
        qeuePreLanding (airplane);
    } else if (airplane.state == 'S') {
        printAirplaneStatus (airplane);
    }
}
void removeAirplane (Airport airport, Airplane airplane) {
	QeueItem* prevItem = runways [airport.runway].qeue [airport.qeueId];
	QeueItem*  currItem = prevItem;
	// Check if curr item is the head of qeue
	if (currItem->airplane.id == airplane.id) {
		 runways [airport.runway].qeue [airport.qeueId] = currItem->nextItem;
		runways [airport.runway].planesQeued -= 1;
		 return;
	}
	
	while (currItem != NULL) {
		if (currItem->nextItem != NULL) {
			if (currItem->airplane.id == airplane.id) {
				prevItem->nextItem = currItem->nextItem;
				runways [airport.runway].planesQeued -= 1;
			//	free (currItem);
			}
			prevItem = currItem;
		}
		currItem = currItem->nextItem;
	}
	
	// Check if item to remove is last in qeue
	currItem = prevItem->nextItem;
	if (currItem->airplane.id == airplane.id) {
		prevItem->nextItem = NULL;
		runways [airport.runway].planesQeued -= 1;
	}
	
}
void handleEmergencyLandings (Airport airport) {
	QeueItem* item;
	item = runways [airport.runway].qeue [LANDING_QEUE];
	while (item != NULL) {
		Airplane airplane = item->airplane;
		if (airplane.fuel == 0) {
			printf ("%s %i with %i passengers successfully made an emergency landing on runway %i at %i:%i. Expected time of arrival was %i:%i.\n",
				airplane.airline, airplane.id, airplane.numPassengers, airport.runway, airport.time / 60, airport.time % 60, 
				(airplane.takeOffTime + airplane.timeUntilLanding) / 60, (airplane.takeOffTime + airplane.timeUntilLanding) % 60   );
			fprintf (out, "%s %i with %i passengers successfully made an emergency landing on runway %i at %i:%i. Expected time of arrival was %i:%i.\n",
				airplane.airline, airplane.id, airplane.numPassengers, airport.runway, airport.time / 60, airport.time % 60, 
				(airplane.takeOffTime + airplane.timeUntilLanding) / 60, (airplane.takeOffTime + airplane.timeUntilLanding) % 60   );
			removeAirplane (airport, airplane);
		}
		airport.time += 1;
		item = item->nextItem;
	}
}
void readyLandings () {
	QeueItem* item = runways [0].qeue [2];
	int oldRunway = airport.runway;
	int oldQeueId = airport.qeueId;
	if (item != NULL) {
		Airplane airplane = item->airplane;
		if (airplane.takeOffTime <= airport.time) {
			airport.runway = 0;
			airport.qeueId = 2;
			pop (airport, runways);
			qeueLanding (airplane);
		}
		airport.runway = oldRunway;
		airport.qeueId = oldQeueId;
	}
}
void updateQeueInfo (Airport airport) {
	QeueItem *item = runways [airport.runway].qeue [airport.qeueId];
	while (item != NULL) {
		item->airplane.timeInQeue += 1;
		if (airport.qeueId == LANDING_QEUE) {
			item->airplane.fuel -= 1;
		}
		item = item->nextItem;
	}
}


void nextInQeue () {
	Airport airplane;
	airport.time += 1;
	// Emergency landings
	int i = 0;
	for (i = 0; i < NUM_RUNWAYS; ++i) {
		airport.runway = i;
		airport.qeueId = LANDING_QEUE;
		handleEmergencyLandings (airport);
	}

	for (i = 0; i < NUM_RUNWAYS; ++i) {
		QeueItem* item;
		Airplane airplane;
		airport.runway = i;
		// Check takeoff qeue
		if (item == NULL)
			continue;
	
		// Check landing qeue
		int i = 0;
		for (i = TAKEOFF_QEUE;
			i <= 1;
			++i) {
				item = runways [airport.runway].qeue [i];
				airport.qeueId = i;
				if (item == NULL) 
					continue;
				airplane = item->airplane;
				if (airplane.state == 'T') {
					int takeOffTime = airplane.takeOffTime + airplane.timeInQeue;
					#ifdef TESTS
						printf ("%s %i, takeOffTime = %i, airportTime = %i\n", airplane.airline, airplane.id, takeOffTime, airport.time);
					#endif
					if (airplane.takeOffTime <= airport.time) {
					printf ("%s %i with %i passengers departed on runway %i at %i:%i. Expected departure time was %i:%i\n"
						, airplane.airline, airplane.id, airplane.numPassengers, airport.runway,
						airport.time / 60, airport.time % 60, airplane.takeOffTime / 60,
						airplane.takeOffTime % 60);
					fprintf (out, "%s %i with %i passengers departed on runway %i at %i:%i. Expected departure time was %i:%i\n"
						, airplane.airline, airplane.id, airplane.numPassengers, airport.runway,
						airport.time / 60, airport.time % 60, airplane.takeOffTime / 60,
						airplane.takeOffTime % 60);
					pop (airport, runways);
					break;
					}
				} else if (airplane.state == 'L') {
					int landingTime = airplane.takeOffTime + airplane.timeUntilLanding + airplane.timeInQeue;
					#ifdef TESTS
						printf ("%s %i, landingTime = %i, airportTime = %i\n", airplane.airline, airplane.id, landingTime, airport.time);
					#endif
					if (airplane.takeOffTime + airplane.timeUntilLanding <= airport.time) {
					printf ("%s %i with %i passengers landed on runway %i at %i:%i. Expected arrival time was %i:%i\n"
						, airplane.airline, airplane.id, airplane.numPassengers, airport.runway,
						airport.time / 60, airport.time % 60, (airplane.takeOffTime + airplane.timeUntilLanding)/ 60,
						(airplane.takeOffTime + airplane.timeUntilLanding) % 60);
					fprintf (out, "%s %i with %i passengers landed on runway %i at %i:%i. Expected arrival time was %i:%i\n"
						, airplane.airline, airplane.id, airplane.numPassengers, airport.runway,
						airport.time / 60, airport.time % 60, airplane.takeOffTime + airplane.timeUntilLanding / 60,
						airplane.takeOffTime + airplane.timeUntilLanding % 60);
					pop (airport, runways);
					break;
					}
				}
				updateQeueInfo (airport);
			}
		}
}

int airportSim (char inFile[], char outFile[]) {
	airport.time = 0;
   int i=0;
     for (i = 0; i < NUM_RUNWAYS; ++i) {
        runways [i].qeue [0] = NULL;
        runways [i].qeue [1]= NULL;
    }
    
    if ((in = fopen (inFile, "r")) == NULL) {
        printf ("Could not open file for reading.\n");
        return -1;
    }
    
    if ((out = fopen (outFile, "w")) == NULL) {
    	printf ("Could not open file for writing.\n");
    	return -1;
	}
    
    int numAirplanes = readFile (in, airplanes);
    for (i = 0; runways [0].planesQeued > 0 || runways [1].planesQeued > 0 || i < numAirplanes;) {
 		if (i < numAirplanes) {
		 	qeueFlight (airplanes [i]);
		 	++i;	
		 }
 		nextInQeue ();
 		readyLandings ();
 	    printRunwayStatus (runways);
 
	}
	system ("pause");
	return 0;
}


#undef TESTS
int main (int argc, char** args) {
	init ();
#ifdef TESTS
 //  	testQeue ();
//    testQeueSelect ();
//    printRunwayQeues ();
//    printf ("Here\n");
//    testEmergencyLanding ();
//    printRunwayQeues ();
//    printf ("Testing Next in Qeue\n");
//    testNextInQeue ();
//    testPrintStatus ();
#endif
	if (argc == 3) {
		airportSim (args [1], args [2]);
	} else {
		if (airportSim ("input2.txt", "output.txt") < 0) {
			usage (args [0]);
		}
			
	}
    system("pause");
}
