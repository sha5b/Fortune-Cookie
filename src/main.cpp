#include <Arduino.h>
#include "Adafruit_Thermal.h"
#include <HardwareSerial.h>
#include "fortunes.h"

// Define pins
const int CIRCUIT_PIN = 4;  // Input pin for circuit detection
const int PRINTER_RX = 16;  // Printer RX pin
const int PRINTER_TX = 17;  // Printer TX pin

// Initialize printer on Serial2
HardwareSerial PrinterSerial(2);  // Use hardware serial 2
Adafruit_Thermal printer(&PrinterSerial);

// Markov Chain structure
struct MarkovNode {
    String word;
    String nextWords[10];  // Store up to 10 possible next words
    int numNextWords;
};

const int MAX_NODES = 200;  // Increased for more unique words
MarkovNode nodes[MAX_NODES];
int numNodes = 0;

// Find or create a node for a word
int findOrCreateNode(String word) {
    // Look for existing node
    for (int i = 0; i < numNodes; i++) {
        if (nodes[i].word == word) {
            return i;
        }
    }
    
    // Create new node if space available
    if (numNodes < MAX_NODES) {
        nodes[numNodes].word = word;
        nodes[numNodes].numNextWords = 0;
        return numNodes++;
    }
    
    return 0; // Return first node if full
}

// Add a transition between words
void addTransition(String word1, String word2) {
    int nodeIndex = findOrCreateNode(word1);
    
    // Add next word if space available and not already present
    if (nodes[nodeIndex].numNextWords < 10) {
        // Check if word2 is already in nextWords
        bool exists = false;
        for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
            if (nodes[nodeIndex].nextWords[i] == word2) {
                exists = true;
                break;
            }
        }
        
        // Add if not already present
        if (!exists) {
            nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords++] = word2;
        }
    }
}

// Build Markov chain from fortune array
void buildMarkovChain() {
    for (int i = 0; i < NUM_FORTUNES; i++) {
        String fortune = fortunes[i];
        String words[30];  // Max 30 words per fortune
        int wordCount = 0;
        
        // Split fortune into words
        int start = 0;
        for (int j = 0; j < fortune.length(); j++) {
            if (fortune[j] == ' ' || j == fortune.length() - 1) {
                String word = fortune.substring(start, j == fortune.length() - 1 ? j + 1 : j);
                words[wordCount++] = word;
                start = j + 1;
            }
        }
        
        // Add transitions between consecutive words
        for (int j = 0; j < wordCount - 1; j++) {
            addTransition(words[j], words[j + 1]);
        }
    }
}

// Generate a random fortune using Markov chain
String generateFortune() {
    // Start with a random word from nodes
    int currentNode = random(numNodes);
    String fortune = nodes[currentNode].word;
    int length = 1;
    
    // Generate up to 20 words
    while (length < 20 && nodes[currentNode].numNextWords > 0) {
        // Pick random next word
        int nextWordIndex = random(nodes[currentNode].numNextWords);
        String nextWord = nodes[currentNode].nextWords[nextWordIndex];
        
        fortune += " " + nextWord;
        length++;
        
        // Find node for next word
        for (int i = 0; i < numNodes; i++) {
            if (nodes[i].word == nextWord) {
                currentNode = i;
                break;
            }
        }
    }
    
    return fortune;
}

// Print text with word wrapping
void printWrappedText(const String& text, int maxWidth = 32) {
    String currentLine;
    String words[30];
    int wordCount = 0;
    
    // Split into words
    int start = 0;
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == ' ' || i == text.length() - 1) {
            words[wordCount++] = text.substring(start, i == text.length() - 1 ? i + 1 : i);
            start = i + 1;
        }
    }
    
    // Build lines
    for (int i = 0; i < wordCount; i++) {
        String testLine = currentLine;
        if (testLine.length() > 0) testLine += " ";
        testLine += words[i];
        
        if (testLine.length() > maxWidth) {
            printer.println(currentLine);
            currentLine = words[i];
        } else {
            currentLine = testLine;
        }
    }
    
    // Print last line if any
    if (currentLine.length() > 0) {
        printer.println(currentLine);
    }
}

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    
    // Initialize printer serial
    PrinterSerial.begin(19200, SERIAL_8N1, PRINTER_RX, PRINTER_TX);
    printer.begin();
    
    // Set up input pin
    pinMode(CIRCUIT_PIN, INPUT_PULLUP);
    
    // Initialize random seed using analog noise
    randomSeed(analogRead(0));
    
    // Build Markov chain
    buildMarkovChain();
    
    Serial.println("Setup complete!");
    Serial.printf("Initialized with %d nodes from %d source fortunes\n", numNodes, NUM_FORTUNES);
}

void loop() {
    // Check if circuit is closed (pulled low)
    if (digitalRead(CIRCUIT_PIN) == LOW) {
        Serial.println("Circuit closed - printing fortune!");
        
        // Generate and print fortune
        String fortune = generateFortune();
        Serial.println("Generated fortune: " + fortune);
        
        printer.wake();       // Wake up printer
        printer.setSize('M'); // Set medium text size
        
        // Print decorative header
        printer.println("╔══════════════════╗");
        printer.println("║   YOUR FORTUNE   ║");
        printer.println("╚══════════════════╝");
        printer.println();
        
        // Print wrapped fortune
        printWrappedText(fortune);
        printer.println();
        
        // Print decorative footer
        printer.println("~ ~ ~ ~ ~ ~ ~ ~ ~ ~");
        printer.feed(3);      // Feed paper 3 lines
        
        printer.sleep();      // Sleep printer
        
        // Delay to prevent multiple triggers
        delay(1000);
    }
    
    delay(100); // Small delay in main loop
}
