#include <Arduino.h>
#include <HardwareSerial.h>
#include "fortunes.h"

// Define pins
const int CIRCUIT_PIN = 4;     // Input pin for circuit detection
const int PRINTER_RX = 32;     // Printer TX pin
const int PRINTER_TX = 33;     // Printer RX pin

// Initialize printer serial directly
HardwareSerial PrinterSerial(2);  // Use hardware serial 2

// State tracking
bool isPrinting = false;
bool lastCircuitState = HIGH;
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 2000;  // Minimum time between prints (2 seconds)

// Markov Chain structure
const int MAX_WORD_LENGTH = 12;    
const int MAX_NODES = 150;         
const int MAX_NEXT_WORDS = 5;      
const int MIN_FORTUNE_WORDS = 6;   
const int MAX_FORTUNE_WORDS = 12;  

struct MarkovNode {
    char word[MAX_WORD_LENGTH];
    struct NextWord {
        uint8_t nodeIndex;
        uint8_t count;
    } nextWords[MAX_NEXT_WORDS];
    uint8_t numNextWords;
    bool canStartSentence;    
    bool canEndSentence;
};

MarkovNode nodes[MAX_NODES];
uint16_t numNodes = 0;

// Function declarations
void printerWrite(const char* str);
void printerNewLine();
int findOrCreateNode(const char* word);
void addTransition(const char* word1, const char* word2, bool isFirstWord, bool isLastWord);
void buildMarkovChain();
const char* selectNextWord(int nodeIndex, const char* currentWord);
String generateFortune();
void printFortune(const String& fortune);

// Basic printer functions
void printerWrite(const char* str) {
    while (*str) {
        PrinterSerial.write(*str++);
        delay(1);  // Small delay between characters
    }
}

void printerNewLine() {
    PrinterSerial.write('\r');
    PrinterSerial.write('\n');
    delay(10);  // Delay after line feed
}

// Find or create a node for a word
int findOrCreateNode(const char* word) {
    for (int i = 0; i < numNodes; i++) {
        if (strcmp(nodes[i].word, word) == 0) {
            return i;
        }
    }
    
    if (numNodes < MAX_NODES) {
        strncpy(nodes[numNodes].word, word, MAX_WORD_LENGTH - 1);
        nodes[numNodes].word[MAX_WORD_LENGTH - 1] = '\0';
        nodes[numNodes].numNextWords = 0;
        nodes[numNodes].canStartSentence = false;
        nodes[numNodes].canEndSentence = false;
        return numNodes++;
    }
    
    return -1;
}

void addTransition(const char* word1, const char* word2, bool isFirstWord, bool isLastWord) {
    int nodeIndex = findOrCreateNode(word1);
    if (nodeIndex == -1) return;
    
    int nextNodeIndex = findOrCreateNode(word2);
    if (nextNodeIndex == -1) return;
    
    if (isFirstWord) {
        nodes[nodeIndex].canStartSentence = true;
    }
    if (isLastWord) {
        nodes[nodeIndex].canEndSentence = true;
    }
    
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        if (nodes[nodeIndex].nextWords[i].nodeIndex == nextNodeIndex) {
            if (nodes[nodeIndex].nextWords[i].count < 255) {
                nodes[nodeIndex].nextWords[i].count++;
            }
            return;
        }
    }
    
    if (nodes[nodeIndex].numNextWords < MAX_NEXT_WORDS) {
        nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords].nodeIndex = nextNodeIndex;
        nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords].count = 1;
        nodes[nodeIndex].numNextWords++;
    }
}

void buildMarkovChain() {
    char wordBuffer[MAX_WORD_LENGTH];
    
    for (int i = 0; i < NUM_FORTUNES; i++) {
        String fortune = getFortune(i);
        
        int start = 0;
        int wordCount = 0;
        const char* words[15];
        
        for (int j = 0; j <= fortune.length(); j++) {
            if (j == fortune.length() || fortune[j] == ' ') {
                if (j - start < MAX_WORD_LENGTH) {
                    fortune.substring(start, j).toCharArray(wordBuffer, MAX_WORD_LENGTH);
                    words[wordCount++] = strdup(wordBuffer);
                    if (wordCount >= 15) break;
                }
                start = j + 1;
            }
        }
        
        for (int j = 0; j < wordCount - 1; j++) {
            addTransition(words[j], words[j + 1], j == 0, j == wordCount - 2);
        }
        
        for (int j = 0; j < wordCount; j++) {
            free((void*)words[j]);
        }
    }
}

const char* selectNextWord(int nodeIndex, const char* currentWord) {
    if (nodes[nodeIndex].numNextWords == 0) return nullptr;
    
    int totalWeight = 0;
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        const char* nextWord = nodes[nodes[nodeIndex].nextWords[i].nodeIndex].word;
        if (strcmp(nextWord, currentWord) != 0) {
            totalWeight += nodes[nodeIndex].nextWords[i].count;
        }
    }
    
    if (totalWeight == 0) return nullptr;
    
    int randomWeight = random(totalWeight);
    
    int currentWeight = 0;
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        const char* nextWord = nodes[nodes[nodeIndex].nextWords[i].nodeIndex].word;
        if (strcmp(nextWord, currentWord) != 0) {
            currentWeight += nodes[nodeIndex].nextWords[i].count;
            if (randomWeight < currentWeight) {
                return nextWord;
            }
        }
    }
    
    return nullptr;
}

String generateFortune() {
    int attempts = 0;
    int currentNode;
    do {
        currentNode = random(numNodes);
        attempts++;
    } while (!nodes[currentNode].canStartSentence && attempts < 50);
    
    String fortune = nodes[currentNode].word;
    int length = 1;
    
    while (length < MAX_FORTUNE_WORDS) {
        if ((length >= MIN_FORTUNE_WORDS && nodes[currentNode].canEndSentence && random(3) == 0) ||
            nodes[currentNode].numNextWords == 0) {
            break;
        }
        
        const char* nextWord = selectNextWord(currentNode, nodes[currentNode].word);
        if (!nextWord) break;
        
        fortune += " ";
        fortune += nextWord;
        length++;
        
        for (int i = 0; i < numNodes; i++) {
            if (strcmp(nodes[i].word, nextWord) == 0) {
                currentNode = i;
                break;
            }
        }
    }
    
    return fortune;
}

void printFortune(const String& fortune) {
    if (isPrinting) return;
    
    isPrinting = true;
    
    // Print with basic serial writes
    printerNewLine();
    printerNewLine();
    printerWrite("YOUR FORTUNE");
    printerNewLine();
    printerNewLine();
    
    // Print fortune text
    char buffer[100];
    fortune.toCharArray(buffer, sizeof(buffer));
    printerWrite(buffer);
    
    printerNewLine();
    printerNewLine();
    printerNewLine();
    
    isPrinting = false;
}

void setup() {
    Serial.begin(115200);
    
    // Initialize printer serial with default settings
    PrinterSerial.begin(9600, SERIAL_8N1, PRINTER_RX, PRINTER_TX);
    delay(500);
    
    pinMode(CIRCUIT_PIN, INPUT_PULLUP);
    
    uint32_t seed = 0;
    for(int i = 0; i < 8; i++) {
        seed = (seed << 4) | (analogRead(0) & 0x0F);
        delay(1);
    }
    randomSeed(seed);
    
    buildMarkovChain();
    
    Serial.println("Setup complete!");
    Serial.printf("Initialized with %d nodes\n", numNodes);
    Serial.println("Ready for circuit trigger on GPIO4");
}

void loop() {
    bool currentState = digitalRead(CIRCUIT_PIN);
    unsigned long currentTime = millis();
    
    // Check for new print trigger
    if (currentState == LOW && lastCircuitState == HIGH && 
        !isPrinting && (currentTime - lastPrintTime >= PRINT_INTERVAL)) {
        
        String fortune = generateFortune();
        Serial.println("Circuit triggered - Printing fortune:");
        Serial.println(fortune);
        
        printFortune(fortune);
        lastPrintTime = currentTime;
    }
    
    lastCircuitState = currentState;
    delay(10);
}
