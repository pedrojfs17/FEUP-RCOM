gcc -Wall -o writer writenoncanonical.c ll.c state.c utils.c && gcc -Wall -o reader noncanonical.c ll.c state.c utils.c