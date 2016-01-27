
/*******************************************************************************
Tests
*******************************************************************************/
void testQeue () {
   int i=0;
     for (i = 0; i < NUM_RUNWAYS; ++i) {
        runways [i].qeue [0] = NULL;
        runways [i].qeue [1]= NULL;
    }
    char inFile[] = "input.txt";
    if ((in = fopen (inFile, "r")) == NULL) {
        printf ("Could not open file.\n");
    }
    
    int numAirplanes = readFile (in, airplanes);
    printf("Printing airplane array\n");
    for (i = 0; i < numAirplanes; ++i)
        printAirplane(airplanes[i]);
	// Test push
    printf ("Pushing items\n");
    airport.runway = 0;
    airport.qeueId = 0;
    for (i = 0; i < 10; ++i) {
      push (airport, runways, airplanes [i]);
    }
    printf ("Removing itmes\n");
    printQeue (airport);
    printf ("Removing airplane with id %i\n", airplanes [0].id);
    printf ("Removing airplane with id %i\n", airplanes [4].id);
	printf ("Removing airplane with id %i\n", airplanes [9].id);
    
    removeAirplane (airport, airplanes [0]);
    removeAirplane (airport, airplanes [4]);
    removeAirplane (airport, airplanes [9]);
    printf ("Finished removing itmes\n");
    printQeue (airport);
    
    // Test pop
    printf ("Poping items\n");
    QeueItem* head = NULL;
    head = pop (airport, runways);
    if (head == NULL) {
    	printf ("Qeue empty.\n");
    	return;
	}
	// Pop items
	while (head != NULL) {
        head = pop (airport, runways);
        if (head != NULL)
			printAirplane (head->airplane);
    }
    //printQeue(airport);
    
    //printRunwayQeues ();
   // testAirportLogic (airplanes);
        
}
void printQeue (Airport airport) {
	QeueItem* item = runways [airport.runway].qeue [airport.qeueId];
    if (item == NULL) {
    	printf ("Qeue empty.\n");
    	return;
	}
	while (item != NULL) {
			printAirplane (item->airplane);
			item = item->nextItem;
    }
}

void printRunwayQeues () {
	int i;
	for (i = 0; i < NUM_RUNWAYS; ++i) {
		printf ("****************Runway %i*******************\n\n", i + 1);
		airport.runway = i;
		printf ("Takeoff qeue.\n");
		airport.qeueId = TAKEOFF_QEUE;
	    printQeue (airport);
	    printf ("Landing quee.\n");
		airport.qeueId = LANDING_QEUE;
		printQeue (airport);
	    printf ("********************************************\n\n");
	}
}
/*
	Test that the qeue with the lowest number of planes in qeue is selected
*/
void testQeueSelect () {
	Airplane airplane;
    int i;
    for (i = 0; i < 10; ++i) {
    	int minRunway= runways [0].planesQeued <= runways [1].planesQeued ?
				0 : 1,
			minQeue = runways [minRunway].planesQeued;
	    strcpy(airplane.airline, "MA"); airplane.id = 1;
	    
	    airplane.state = 'T'; airplane.takeOffTime = rand () % 6 + 1;
	    qeueFlight (airplane);
	    
	    if (minQeue + 1 != runways [minRunway].planesQeued) {
	    	printf ("TestQeueSelect failed\n");
		}
		printf("Minqeue = %i, Runway %i, planesqeued = %i\n",
			 minQeue, airport.runway, runways [airport.runway].planesQeued);
		
	}
}
void testPrintStatus () {
	Airplane airplane;
	int i;
	
	srand (time (NULL));
	for (i = 0; i < 6; ++i) {
		strcpy (airplane.airline, "MA");
		airplane.id = i;
		airplane.takeOffTime = i;
		airplane.numPassengers = 200;
		airport.runway = rand () % 2;
		airport.qeueId = (rand ()) % 2;
		
		if (airport.qeueId == 0) {
			airplane.state = 'T';
		} else {
			airplane.state = 'L';
			airplane.timeUntilLanding = rand () % 6 + 1;
			airplane.fuel = rand () % 4;
		}
		push (airport, runways, airplane);
	}
	
	Airplane airplane2;
	for (i = 0; i < 6; ++i) {
		airplane2.id = i;
		printAirplaneStatus (airplane2);
	}
	
}
void testEmergencyLanding () {
	Airplane airplane;
    int i;
    airport.runway = 0;
    for (i = 0; i < 9; ++i) {
    	strcpy (airplane.airline, "MA");
    	airplane.id = i;
	    airplane.state = 'L'; airplane.takeOffTime = rand () %  + 1;
	    airplane.takeOffTime = i;
	    airplane.timeUntilLanding = i + 1;
	    airplane.numPassengers = 200;
	    airport.time = i;
	    if (i%2 == 0) {
	    	airplane.fuel = 1;
		} else {
			airplane.fuel = 0;
		}
		push (airport, runways, airplane);
	}
	handleEmergencyLandings (airport);
}
void testNextInQeue () {
	int i, time = 0; 
	airport.time = 0;
	Airplane airplane;
	for (i = 0; i < NUM_RUNWAYS; ++i) {
        runways [i].qeue [0] = NULL;
        runways [i].qeue [1]= NULL;
    }
	for (i = 0; i < 9; ++i) {
    	strcpy (airplane.airline, "MA");
    	airplane.id = i;
	    airplane.takeOffTime = time + rand () % 4 + 1;
	    time = airplane.takeOffTime;
	    airplane.numPassengers = 200;
	    if (i%2 == 0) {
	    	airplane.state = 'T';
		} else {
			airplane.timeUntilLanding = rand() % 4 + 1;
			airplane.takeOffTime -= airplane.timeUntilLanding;
			airplane.state = 'L';
	    	if (rand () % 2 == 0)
	    		airplane.fuel = 0;
	    	else
	    		airplane.fuel = 1;
		}
		qeueFlight (airplane);
	}
	
	for (airport.time; airport.time <= 1000; ++airport.time) {
		nextInQeue ();
		readyLandings ();
	}
	printRunwayQeues ();
}


