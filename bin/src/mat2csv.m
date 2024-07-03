function mat2csv(matfile,csvfile)
  A=load(matfile);
  T=struct2table(A);
  writetable(T,csvfile);
