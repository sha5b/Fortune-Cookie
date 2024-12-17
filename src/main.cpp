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

// Markov Chain structure - Increased limits for more complex content
const int MAX_WORD_LENGTH = 15;    // Increased from 12 to allow longer words
const int MAX_NODES = 200;         // Increased from 150 to allow more unique words
const int MAX_NEXT_WORDS = 8;      // Increased from 5 to allow more varied combinations
const int MIN_FORTUNE_WORDS = 5;   // Decreased to allow shorter, punchier sentences
const int MAX_FORTUNE_WORDS = 18;  // Increased to allow longer philosophical statements

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

// Printer commands
void printerInit() {
    // Initialize printer
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('@');  // Initialize command
    delay(50);
    
    // Set print temperature to 32 degrees
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write(55);  // Heat setting command
    PrinterSerial.write(32);  // Temperature value (32)
    PrinterSerial.write(3);   // Heat time (standard)
    delay(50);
    
    // Set smaller text size for better paper efficiency
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('!');  // Text size command
    PrinterSerial.write(10);  // Reduced size for better efficiency
    delay(50);
    
    // Set tighter line spacing
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('3');  // Line spacing command
    PrinterSerial.write(25);  // Reduced spacing for better efficiency
    delay(50);
}

void printerWrite(const char* str) {
    while (*str) {
        PrinterSerial.write(*str++);
        delay(2);  // Increased delay between characters
    }
}

void printerNewLine() {
    PrinterSerial.write('\r');
    PrinterSerial.write('\n');
    delay(50);  // Increased delay after line feed
}

void printerFeed(int lines) {
    for (int i = 0; i < lines; i++) {
        printerNewLine();
    }
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
        const char* words[25];  // Increased buffer for longer sentences
        
        for (int j = 0; j <= fortune.length(); j++) {
            if (j == fortune.length() || fortune[j] == ' ') {
                if (j - start < MAX_WORD_LENGTH) {
                    fortune.substring(start, j).toCharArray(wordBuffer, MAX_WORD_LENGTH);
                    words[wordCount++] = strdup(wordBuffer);
                    if (wordCount >= 25) break;
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
    
    // Randomly choose target length between MIN and MAX
    int targetLength = random(MIN_FORTUNE_WORDS, MAX_FORTUNE_WORDS + 1);
    
    while (length < MAX_FORTUNE_WORDS) {
        // Dynamically adjust end probability based on current length vs target
        bool shouldEnd = false;
        if (length >= MIN_FORTUNE_WORDS) {
            if (length >= targetLength) {
                shouldEnd = nodes[currentNode].canEndSentence;
            } else {
                shouldEnd = nodes[currentNode].canEndSentence && random(5) == 0;
            }
        }
        
        if (shouldEnd || nodes[currentNode].numNextWords == 0) {
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
    
    // Initialize printer with smaller text
    printerInit();
    
    // Print with minimal spacing
    printerFeed(1);
    
    // Print title with medium text size
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('!');  // Text size command
    PrinterSerial.write(30);  // Reduced title size for better efficiency
    printerWrite("YOUR FORTUNE");
    printerFeed(1);
    
    // Reset to normal fortune text size
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('!');  // Text size command
    PrinterSerial.write(10);  // Smaller size for better efficiency
    
    // Print fortune text with word wrapping
    char buffer[150];  // Increased buffer for longer fortunes
    fortune.toCharArray(buffer, sizeof(buffer));
    
    // Split text into words and print with proper spacing
    char* word = strtok(buffer, " ");
    int lineLength = 0;
    
    while (word != NULL) {
        int wordLen = strlen(word);
        
        // Check if we need to start a new line (adjusted for new font size)
        if (lineLength + wordLen + 1 > 24) {  // Increased character limit due to smaller font
            printerNewLine();  // Single line feed between wrapped lines
            lineLength = 0;
        }
        
        if (lineLength > 0) {
            printerWrite(" ");
            lineLength++;
        }
        
        printerWrite(word);
        lineLength += wordLen;
        
        word = strtok(NULL, " ");
    }
    
    // Add minimal feed at the end
    printerFeed(4);
    
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
