# Makefile

NAME = ravenc

HSCC = ghc

HS_SRC = $(shell find src/haskell -not -path "*/tests*" -name "*.hs")

CPP_SRC = $(shell find src/cpp -name "*.cpp")

IFLAGS = -I`ghc --print-libdir`/include -I./include
LIBS = -lstdc++

all: $(NAME)

compile_haskell: $(HS_SRC)
	$(HSCC) -c -O $(HS_SRC)
	python script/trim.py `find src/haskell -name "*.h"`
	rm -f `find src/haskell -name "*.h"`

$(NAME): compile_haskell
	$(HSCC) --make -no-hs-main -optc-O $(CPP_SRC) main.cpp `find src/haskell -name "*.o"` -o $(NAME) $(LIBS) $(IFLAGS)

clean:
	rm -f `find . -name "*.o"`
	rm -f `find src/haskell -name "*.hi"`
	rm -f `find src/haskell -name "*.h"`

fclean: clean
	rm -f $(NAME)

re: fclean all

syntax: $(HS_SRC)
	$(HSCC) -c $(HS_SRC) Main.hs -package megaparsec
	$(HSCC) `find src/haskell -name "*.o"` Main.o -o $(NAME) -package megaparsec

syntax-test: $(HS_SRC)
	$(HSCC) -c $(HS_SRC) -package megaparsec -package hspec -package hspec-megaparsec
	$(HSCC) `find src/haskell -name "*.o"` -o $(NAME)-test -package megaparsec -package hspec -package hspec-megaparsec
	./$(NAME)-test
