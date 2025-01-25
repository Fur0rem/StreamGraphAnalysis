# split:
# 	gcc split.c -o bin/split -L../bin -lSGA -lm

# metrics:
# 	gcc metrics.c -o bin/metrics -L../bin/release -lSGA -lm

# analysis:
# 	gcc analysis.c -o bin/analysis -L../bin/release -lSGA -lm

# time_evolution:
# 	gcc time_evolution.c -o bin/time_evolution -L../bin/release -lSGA -lm

%: %.c | bin
	gcc $< -o bin/$@ -L../bin/release -lSGA -lm