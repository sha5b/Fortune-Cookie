#include <Arduino.h>
#include "Adafruit_Thermal.h"
#include <HardwareSerial.h>
#include "fortunes.h"

// Define pins
const int CIRCUIT_PIN = 4;     // Input pin for circuit detection
const int PRINTER_RX = 16;     // Printer RX pin
const int PRINTER_TX = 17;     // Printer TX pin

// Initialize printer on Serial2
HardwareSerial PrinterSerial(2);  // Use hardware serial 2
Adafruit_Thermal printer(&PrinterSerial);

// Optimized Markov Chain structure
const int MAX_WORD_LENGTH = 12;    // Reduced from 16
const int MAX_NODES = 150;         // Reduced from 200
const int MAX_NEXT_WORDS = 5;      // Reduced from 10
const int MIN_FORTUNE_WORDS = 6;   // Minimum words in generated fortune
const int MAX_FORTUNE_WORDS = 12;  // Maximum words in generated fortune

struct MarkovNode {
    char word[MAX_WORD_LENGTH];
    struct NextWord {
        uint8_t nodeIndex;
        uint8_t count;
    } nextWords[MAX_NEXT_WORDS];
    uint8_t numNextWords;
    bool canStartSentence;    // New flag for sentence starters
    bool canEndSentence;
};

MarkovNode nodes[MAX_NODES];
uint16_t numNodes = 0;

// Circuit state tracking
bool lastCircuitState = HIGH;
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 2000;  // Minimum time between prints (2 seconds)

// Find or create a node for a word
int findOrCreateNode(const char* word) {
    // Look for existing node
    for (int i = 0; i < numNodes; i++) {
        if (strcmp(nodes[i].word, word) == 0) {
            return i;
        }
    }
    
    // Create new node if space available
    if (numNodes < MAX_NODES) {
        strncpy(nodes[numNodes].word, word, MAX_WORD_LENGTH - 1);
        nodes[numNodes].word[MAX_WORD_LENGTH - 1] = '\0';
        nodes[numNodes].numNextWords = 0;
        nodes[numNodes].canStartSentence = false;
        nodes[numNodes].canEndSentence = false;
        return numNodes++;
    }
    
    return -1; // Return error if full
}

// Add a transition between words
void addTransition(const char* word1, const char* word2, bool isFirstWord, bool isLastWord) {
    int nodeIndex = findOrCreateNode(word1);
    if (nodeIndex == -1) return;  // Skip if full
    
    int nextNodeIndex = findOrCreateNode(word2);
    if (nextNodeIndex == -1) return;  // Skip if full
    
    // Update sentence position flags
    if (isFirstWord) {
        nodes[nodeIndex].canStartSentence = true;
    }
    if (isLastWord) {
        nodes[nodeIndex].canEndSentence = true;
    }
    
    // Look for existing transition
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        if (nodes[nodeIndex].nextWords[i].nodeIndex == nextNodeIndex) {
            if (nodes[nodeIndex].nextWords[i].count < 255) {
                nodes[nodeIndex].nextWords[i].count++;
            }
            return;
        }
    }
    
    // Add new transition if space available
    if (nodes[nodeIndex].numNextWords < MAX_NEXT_WORDS) {
        nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords].nodeIndex = nextNodeIndex;
        nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords].count = 1;
        nodes[nodeIndex].numNextWords++;
    }
}

// Build Markov chain from fortune array
void buildMarkovChain() {
    char wordBuffer[MAX_WORD_LENGTH];
    
    for (int i = 0; i < NUM_FORTUNES; i++) {
        String fortune = getFortune(i);
        
        int start = 0;
        int wordCount = 0;
        const char* words[15];  // Reduced buffer size
        
        // Split fortune into words
        for (int j = 0; j <= fortune.length(); j++) {
            if (j == fortune.length() || fortune[j] == ' ') {
                if (j - start < MAX_WORD_LENGTH) {
                    fortune.substring(start, j).toCharArray(wordBuffer, MAX_WORD_LENGTH);
                    words[wordCount++] = strdup(wordBuffer);
                    if (wordCount >= 15) break;  // Prevent buffer overflow
                }
                start = j + 1;
            }
        }
        
        // Add transitions between consecutive words
        for (int j = 0; j < wordCount - 1; j++) {
            addTransition(words[j], words[j + 1], j == 0, j == wordCount - 2);
        }
        
        // Free allocated memory
        for (int j = 0; j < wordCount; j++) {
            free((void*)words[j]);
        }
    }
}

// Select next word based on weighted probability
const char* selectNextWord(int nodeIndex, const char* currentWord) {
    if (nodes[nodeIndex].numNextWords == 0) return nullptr;
    
    // Calculate total weight excluding repetitions
    int totalWeight = 0;
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        const char* nextWord = nodes[nodes[nodeIndex].nextWords[i].nodeIndex].word;
        if (strcmp(nextWord, currentWord) != 0) {  // Prevent repetition
            totalWeight += nodes[nodeIndex].nextWords[i].count;
        }
    }
    
    if (totalWeight == 0) return nullptr;  // No valid next words
    
    // Select random weight
    int randomWeight = random(totalWeight);
    
    // Find corresponding word
    int currentWeight = 0;
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        const char* nextWord = nodes[nodes[nodeIndex].nextWords[i].nodeIndex].word;
        if (strcmp(nextWord, currentWord) != 0) {  // Prevent repetition
            currentWeight += nodes[nodeIndex].nextWords[i].count;
            if (randomWeight < currentWeight) {
                return nextWord;
            }
        }
    }
    
    return nullptr;
}

// Generate a random fortune using enhanced Markov chain
String generateFortune() {
    // Start with a random word that can start a sentence
    int attempts = 0;
    int currentNode;
    do {
        currentNode = random(numNodes);
        attempts++;
    } while (!nodes[currentNode].canStartSentence && attempts < 50);
    
    String fortune = nodes[currentNode].word;
    int length = 1;
    
    // Generate fortune with better ending conditions
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
        
        // Find node for next word
        for (int i = 0; i < numNodes; i++) {
            if (strcmp(nodes[i].word, nextWord) == 0) {
                currentNode = i;
                break;
            }
        }
    }
    
    return fortune;
}

// Print text with word wrapping
void printWrappedText(const String& text) {
    int start = 0;
    int lastSpace = -1;
    const int maxWidth = 32;  // Maximum characters per line
    
    for (int i = 0; i <= text.length(); i++) {
        if (i == text.length() || text[i] == ' ') {
            if (i - start > maxWidth) {
                if (lastSpace != -1) {
                    printer.println(text.substring(start, lastSpace));
                    start = lastSpace + 1;
                } else {
                    printer.println(text.substring(start, i));
                    start = i + 1;
                }
            }
            lastSpace = i;
        }
    }
    
    if (start < text.length()) {
        printer.println(text.substring(start));
    }
}

void printAndLogFortune() {
    String fortune = generateFortune();
    Serial.println("Circuit triggered - Printing fortune:");
    Serial.println(fortune);
    
    printer.wake();
    printer.setSize('M');
    
    printer.println("╔══════════════════╗");
    printer.println("║   YOUR FORTUNE   ║");
    printer.println("╚══════════════════╝");
    printer.println();
    
    printWrappedText(fortune);
    printer.println();
    printer.println("~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
    printer.feed(3);
    
    printer.sleep();
}

void setup() {
    Serial.begin(115200);
    
    PrinterSerial.begin(19200, SERIAL_8N1, PRINTER_RX, PRINTER_TX);
    printer.begin();
    
    pinMode(CIRCUIT_PIN, INPUT_PULLUP);  // Enable internal pullup for circuit detection
    
    // Initialize random seed using multiple sources
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
    // Read current circuit state
    bool currentState = digitalRead(CIRCUIT_PIN);
    unsigned long currentTime = millis();
    
    // Check if circuit is closed (LOW) and enough time has passed since last print
    if (currentState == LOW && (currentTime - lastPrintTime >= PRINT_INTERVAL)) {
        printAndLogFortune();
        lastPrintTime = currentTime;
    }
    
    lastCircuitState = currentState;
    delay(10);  // Small delay in main loop
}
