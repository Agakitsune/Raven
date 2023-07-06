# Makefile

NAME = ravenc

HSCC = ghc

HS_SRC = $(shell find src/haskell -not -path "*/tests*" -name "*.hs")

CPP_SRC = $(shell find src/cpp -name "*.cpp")

PCK = -package megaparsec -package utf8-string

IFLAGS = -I`ghc --print-libdir`/include -I./include
LIBS = -lstdc++

all: $(NAME)

compile_haskell: $(HS_SRC)
	$(HSCC) -c -O $(HS_SRC) $(PCK)
	python script/trim.py `find src/haskell -name "*.h"`

$(NAME): compile_haskell
	$(HSCC) $(PCK) --make -no-hs-main -optc-O $(CPP_SRC) main.cpp `find src/haskell -name "*.o"` -o $(NAME) $(LIBS) $(IFLAGS)

clean:
	rm -f `find . -name "*.o"`
	rm -f `find src/haskell -name "*.hi"`
	rm -f `find src/haskell -name "*.h"`

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME)-test
	rm -f $(NAME)-serial

re: fclean all

syntax: $(HS_SRC)
	$(HSCC) -c $(HS_SRC) Main.hs $(PCK)
	$(eval HS_OBJ := $(shell find src/haskell -not -path "*/tests*" -name "*.o"))
	$(HSCC) $(HS_OBJ) Main.o -o $(NAME) $(PCK)

serialize: $(HS_SRC)
	$(HSCC) -c $(HS_SRC) Serialize.hs $(PCK)
	$(eval HS_OBJ := $(shell find src/haskell -not -path "*/tests*" -name "*.o"))
	$(HSCC) $(HS_OBJ) Serialize.o -o $(NAME)-serial $(PCK)

syntax-test: $(HS_SRC)
	$(HSCC) -c $(HS_SRC) src/haskell/tests/Test.hs $(PCK) -package hspec -package hspec-megaparsec
	$(HSCC) `find src/haskell -name "*.o"` -o $(NAME)-test $(PCK) -package hspec -package hspec-megaparsec
	./$(NAME)-test
