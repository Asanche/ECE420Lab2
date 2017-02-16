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
Y1 = ['mutex - count:', num2str(count1), '  mean:', num2str(stats1.mean), '  median:', num2str(stats1.median), '  min:', num2str(stats1.min), '  max:', num2str(stats1.max)]
Y2 = ['mutex - count:', num2str(count2), '  mean:', num2str(stats2.mean), '  median:', num2str(stats2.median), '  min:', num2str(stats2.min), '  max:', num2str(stats2.max)]
Y3 = ['mutex - count:', num2str(count3), '  mean:', num2str(stats3.mean), '  median:', num2str(stats3.median), '  min:', num2str(stats3.min), '  max:', num2str(stats3.max)]
disp(Y1);
disp(Y2);
disp(Y3);
fclose(file1);
fclose(file2);
fclose(file3);