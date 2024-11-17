CC = gcc 
CFLAGS = -Wvla -Wextra -Werror
LDFLAGS = -lm

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

all: dir $(BIN_DIR)/master $(BIN_DIR)/atomo $(BIN_DIR)/alimentatore $(BIN_DIR)/attivatore $(BIN_DIR)/inibitore

# File oggetto
OBJS = $(BUILD_DIR)/master.o $(BUILD_DIR)/atomo.o $(BUILD_DIR)/alimentatore.o $(BUILD_DIR)/attivatore.o $(BUILD_DIR)/inibitore.o $(BUILD_DIR)/InputReader.o

# Regole per i file oggetto
$(BUILD_DIR)/master.o: $(SRC_DIR)/master.c $(INCLUDE_DIR)/master.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/atomo.o: $(SRC_DIR)/atomo.c $(INCLUDE_DIR)/atomo.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/alimentatore.o: $(SRC_DIR)/alimentatore.c $(INCLUDE_DIR)/alimentatore.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/attivatore.o: $(SRC_DIR)/attivatore.c $(INCLUDE_DIR)/attivatore.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/inibitore.o: $(SRC_DIR)/inibitore.c $(INCLUDE_DIR)/inibitore.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/InputReader.o: $(SRC_DIR)/InputReader.c $(INCLUDE_DIR)/inputReader.h
	$(CC) $(CFLAGS) -c $< -o $@

# Regole per gli eseguibili
$(BIN_DIR)/master: $(BUILD_DIR)/master.o $(BUILD_DIR)/InputReader.o
	$(CC) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/atomo: $(BUILD_DIR)/atomo.o
	$(CC) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/alimentatore: $(BUILD_DIR)/alimentatore.o
	$(CC) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/attivatore: $(BUILD_DIR)/attivatore.o
	$(CC) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/inibitore: $(BUILD_DIR)/inibitore.o
	$(CC) $< -o $@ $(LDFLAGS)



dir:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

run:
	./$(BIN_DIR)/master

clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/*
