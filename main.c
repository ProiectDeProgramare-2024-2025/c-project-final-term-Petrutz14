#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_CARDS 100
#define FILE_NAME "cards.dat"

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"

struct card {
    int id, cvc, views, balance;
    char name[32];
    char expdate[6];
    char bank[32];
    char ccnr[20]; // Changed to string to hold "1234 5678 9999 1111"
};

struct card cards[MAX_CARDS];
int cardCount = 0;
int nextCardID = 1;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause(int seconds) {
    Sleep(seconds * 1000);
}

void loadCardsFromFile() {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file != NULL) {
        fread(&cardCount, sizeof(int), 1, file);
        fread(cards, sizeof(struct card), cardCount, file);
        nextCardID = cardCount + 1;
        fclose(file);
    }
}

void saveCardsToFile() {
    FILE *file = fopen(FILE_NAME, "wb");
    if (file != NULL) {
        fwrite(&cardCount, sizeof(int), 1, file);
        fwrite(cards, sizeof(struct card), cardCount, file);
        fclose(file);
    } else {
        printf(RED "Error saving data to file.\n" RESET);
    }
}

void addCard() {
    clearScreen();
    struct card newCard;
    newCard.id = nextCardID++;

    getchar();
    printf("Enter cardholder name: ");
    fgets(newCard.name, sizeof(newCard.name), stdin);
    newCard.name[strcspn(newCard.name, "\n")] = '\0';

    printf("Enter expiration date (MM/YY): ");
    fgets(newCard.expdate, sizeof(newCard.expdate), stdin);
    newCard.expdate[strcspn(newCard.expdate, "\n")] = '\0';

    printf("Enter bank name: ");
    fgets(newCard.bank, sizeof(newCard.bank), stdin);
    newCard.bank[strcspn(newCard.bank, "\n")] = '\0';

    printf("Enter credit card number (format: 1234 5678 9012 3456): ");
    fgets(newCard.ccnr, sizeof(newCard.ccnr), stdin);
    newCard.ccnr[strcspn(newCard.ccnr, "\n")] = '\0';

    printf("Enter CVC: ");
    scanf("%d", &newCard.cvc);

    newCard.balance = 0;
    newCard.views = 0;

    cards[cardCount++] = newCard;
    saveCardsToFile();

    printf(GREEN "\nCard added successfully!\n" RESET);
    pause(3);
}

void viewCards() {
    clearScreen();

    if (cardCount == 0) {
        printf(YELLOW "\nNo cards available.\n" RESET);
        return;
    }

    // Sort by view count descending
    for (int i = 0; i < cardCount - 1; i++) {
        for (int j = i + 1; j < cardCount; j++) {
            if (cards[i].views < cards[j].views) {
                struct card temp = cards[i];
                cards[i] = cards[j];
                cards[j] = temp;
            }
        }
    }

    printf(BOLD CYAN "\nAvailable Cards (ID, Card Number, and Name):\n" RESET);
    for (int i = 0; i < cardCount; i++) {
        printf("ID: %d | Card Number: %s | Cardholder: %s\n", cards[i].id, cards[i].ccnr, cards[i].name);
        printf("----------------------------\n");
    }

    printf("Enter card ID to view full details: ");
    int id;
    scanf("%d", &id);

    int index = -1;
    for (int i = 0; i < cardCount; i++) {
        if (cards[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(RED "\nCard with ID %d not found.\n" RESET, id);
        return;
    }

    // Show full info
    printf(BOLD GREEN "\nCard Details:\n" RESET);
    printf("Card ID: %d\n", cards[index].id);
    printf("Card Number: %s\n", cards[index].ccnr);
    printf("Cardholder: %s\n", cards[index].name);
    printf("Bank: %s\n", cards[index].bank);
    printf("Expiration Date: %s\n", cards[index].expdate);
    printf("CVC: %d\n", cards[index].cvc);
    printf("Balance: %d\n", cards[index].balance);
    printf("Views: %d\n", cards[index].views);
    printf("----------------------------\n");

    cards[index].views++;
    saveCardsToFile();

    printf("\nPress Enter to go back to the menu...\n");
    getchar(); getchar(); // pause
}

void deleteCard() {
    clearScreen();

    int id;
    printf("Enter the ID of the card to delete: ");
    scanf("%d", &id);

    int index = -1;
    for (int i = 0; i < cardCount; i++) {
        if (cards[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(RED "\nCard with ID %d not found.\n" RESET, id);
        return;
    }

    for (int i = index; i < cardCount - 1; i++) {
        cards[i] = cards[i + 1];
    }

    cardCount--;
    saveCardsToFile();

    printf(GREEN "\nCard deleted successfully!\n" RESET);
    pause(3);
}

void withdrawMoney() {
    clearScreen();
    int id, amount;

    printf("Enter the ID of the card to withdraw from: ");
    scanf("%d", &id);

    int index = -1;
    for (int i = 0; i < cardCount; i++) {
        if (cards[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(RED "\nCard with ID %d not found.\n" RESET, id);
        return;
    }

    printf("Enter amount to withdraw: ");
    scanf("%d", &amount);

    if (cards[index].balance >= amount) {
        cards[index].balance -= amount;
        saveCardsToFile();
        printf(GREEN "\nWithdrawal successful! New balance: %d\n" RESET, cards[index].balance);
    } else {
        printf(RED "\nInsufficient balance.\n" RESET);
    }

    pause(3);
}

void depositMoney() {
    clearScreen();
    int id, amount;

    printf("Enter the ID of the card to deposit to: ");
    scanf("%d", &id);

    int index = -1;
    for (int i = 0; i < cardCount; i++) {
        if (cards[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(RED "\nCard with ID %d not found.\n" RESET, id);
        return;
    }

    printf("Enter amount to deposit: ");
    scanf("%d", &amount);

    cards[index].balance += amount;
    saveCardsToFile();

    printf(GREEN "\nDeposit successful! New balance: %d\n" RESET, cards[index].balance);
    pause(3);
}

void displayMenu() {
    int choice;

    do {
        clearScreen();
        printf(BOLD CYAN "====== Card Management System ======\n" RESET);
        printf(BOLD GREEN "1. Add Card\n" RESET);
        printf(BOLD YELLOW "2. View Cards\n" RESET);
        printf(BOLD RED "3. Delete Card\n" RESET);
        printf(BOLD BLUE "4. Withdraw Money\n" RESET);
        printf(BOLD MAGENTA "5. Deposit Money\n" RESET);
        printf(BOLD "6. Exit\n" RESET);
        printf(BOLD CYAN "Choose an option: " RESET);
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addCard();
                break;
            case 2:
                viewCards();
                break;
            case 3:
                deleteCard();
                break;
            case 4:
                withdrawMoney();
                break;
            case 5:
                depositMoney();
                break;
            case 6:
                printf(GREEN "Exiting program...\n" RESET);
                break;
            default:
                printf(RED "Invalid choice, please try again.\n" RESET);
                pause(2);
                break;
        }

    } while (choice != 6);
}

int main() {
    loadCardsFromFile();
    displayMenu();
    return 0;
}
