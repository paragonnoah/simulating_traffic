#include "trafficSimulator.h"

// Optional new function declarations
int maxInt(int a, int b);

void printNames() {
    // Fill in you and your partner's names here, or "N/A" if you worked alone
    printf("\nThis solution was completed by:\n");
    printf("Your Name\n");
    printf("Partner's Name or 'N/A'\n\n");
}


TrafficData* createTrafficData(char* filename) {
    FILE* pFile = fopen(filename, "r");
    if (pFile == NULL) {
        printf("ERROR - the file \"%s\" was not found!\n", filename);
        exit(-1);
    }

    int numVertices, numRoads;
    fscanf(pFile, "%d %d", &numVertices, &numRoads);
    Graph* graph = createGraph(numVertices);
    if (!graph) {
        fprintf(stderr, "Failed to create graph\n");
        fclose(pFile);
        return NULL;
    }

    TrafficData* trafficData = malloc(sizeof(TrafficData));
    if (!trafficData) {
        fprintf(stderr, "Failed to allocate memory for traffic data\n");
        fclose(pFile);
        freeGraph(graph);
        return NULL;
    }

    trafficData->arrayOfRoads = malloc(sizeof(RoadData*) * numRoads);
    if (!trafficData->arrayOfRoads) {
        fprintf(stderr, "Failed to allocate memory for road data array\n");
        free(trafficData);
        freeGraph(graph);
        fclose(pFile);
        return NULL;
    }

    trafficData->numRoads = numRoads;
    trafficData->roadGraph = graph;

    for (int i = 0; i < numRoads; i++) {
        int from, to, length, greenOn, greenOff, cycleReset;
        fscanf(pFile, "%d %d %d %d %d %d", &from, &to, &length, &greenOn, &greenOff, &cycleReset);
        RoadData* road = createRoadData(from, to, length, greenOn, greenOff, cycleReset);
        if (!road) {
            // Clean up previously allocated memory if road creation fails
            fprintf(stderr, "Failed to create road data\n");
            for (int j = 0; j < i; j++) {
                freeRoadData(trafficData->arrayOfRoads[j]);
            }
            free(trafficData->arrayOfRoads);
            free(trafficData);
            freeGraph(graph);
            fclose(pFile);
            return NULL;
        }
        setEdge(graph, from, to, length);
        setEdgeData(graph, from, to, road);
        trafficData->arrayOfRoads[i] = road;
    }

    fclose(pFile);
    return trafficData;
}


void trafficSimulator(TrafficData* pTrafficData) {
    printf("Starting simulation...\n");
    int step = 0;
    while (!isEmptyPQ(pTrafficData->eventQueue)) {
        printf("Step: %d\n", step);
        processEvents(pTrafficData, step);
        updateTrafficLights(pTrafficData, step);
        moveCars(pTrafficData);
        step++;
    }
    calculateResults(pTrafficData);
    printf("Simulation completed.\n");
}

void freeTrafficData(TrafficData* pTrafficData) {
    for (int i = 0; i < pTrafficData->numRoads; i++) {
        freeRoadData(pTrafficData->arrayOfRoads[i]);
    }
    free(pTrafficData->arrayOfRoads);
    freeGraph(pTrafficData->roadGraph);
    free(pTrafficData);
}

int maxInt(int a, int b) {
    return (a > b) ? a : b;
}
