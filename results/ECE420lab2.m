file1 = fopen('mutex_results.txt', 'r');
file2 = fopen('array_results.txt', 'r');
file3 = fopen('rwl_results.txt', 'r');
[X1, count1] = fscanf(file1, '%f\n');
[X2, count2] = fscanf(file2, '%f\n');
[X3, count3] = fscanf(file3, '%f\n');
hold on
[h1, stats1] = cdfplot(X1);
[h2, stats2] = cdfplot(X2);
[h3, stats3] = cdfplot(X3);
legend('mutex', 'array', 'rwl');
disp(['mutex - count:', count1, '  mean:', stats1.mean, '  median:', stats1.median, '  min:', stats1.min, '  max:', stats1.max]);
disp(['mutex - count:', count2, '  mean:', stats2.mean, '  median:', stats2.median, '  min:', stats2.min, '  max:', stats2.max]);
disp(['mutex - count:', count3, '  mean:', stats3.mean, '  median:', stats3.median, '  min:', stats3.min, '  max:', stats3.max]);
fclose(file1);
fclose(file2);
fclose(file3);