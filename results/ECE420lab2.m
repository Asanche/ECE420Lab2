file = fopen('rwl_results.txt', 'r');
[X, count] = fscanf(file, '%f\n');
[h, stats] = cdfplot(X);
hold on
count
stats.mean
stats.median
stats.min
stats.max
fclose(file);