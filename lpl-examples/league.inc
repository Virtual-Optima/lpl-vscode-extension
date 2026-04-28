/* leagueA.lpl :: general include file of I/O functions in league.lpl */

  /** Main data of the league model */
  string parameter PlanFile 'incl':=['test12'] "pla-file";
 
  parameter NNR  "number of round robins";
            NMO  "play mode: 1-8";
            ABZ  "nr matches separation";
            PEN  "penalty threshold for soft constraints";
            DEB  "color in PDF=1, 0=do not color";
            picH; picW "icon height/width on top of PDF page";
            SolverStatus "solved 1-7: 6,7 ok";
            SolverObj    "objective value";

  set bc:=[1..10]      "Break conditions switches";
  integer parameter BC{bc};

  set i,j,j1   "Teams";
  parameter group{i}    "no used"; 
            strength{i} "not used"; 
            showI{i}    "teams shown in output";
  string na{i} "shortname"; 
         name{i}; 
         location{i}; 
         Islot{i} "preferred slot for each team";
  StringToSet(name,i);
  parameter N    "number of teams"; 
            N1   "nr teams minus 1";
  parameter DUM  "the dummy team number";
  parameter dist{i,j} :=1 "distance table";

  set k,k1,k2   "Rounds";
  set K{k}      "single (or double) round-robin subset of k";
  parameter RX{k} "slot sequence in second RoRo";

  set w:=[1..7]    "Weekdays and slots";  
      s:=[1..10]   "time slots per day"; 
      ws{w,s}      "possible slot in weekday"; 
      kw{k,w}      "weekdays of a round"; 
      kws{k,w,s}   "possible slots"; 
      kij{k,i,j}   "blocked match"; 
      kijws{k,i,j,w,s} "blocked slot";
      iws{i,w,s}   "preferred slot for i";
  parameter minsp{w}; maxsp{w} "min/max in weekday";
    date1{k,w}     "date in Table 6 (Calendar)";
    date2{k,i,j}   "date in Table 7 (blocked games)";
    block{k,i,j}   "all blocked games (Table 7)";
    KW{w}          "nr of rounds in weekday w";
  string W{w}      "weekday shortname"; 
         Zeit{w}   "list of match time"; 
         zeit{w,s} "time splitted on slots"; 
         comm3{k,i,j} "comment in match table";
    
  set jj  "Some output strings (Table 1 of PLA-file)";
  string parameter JJ{jj};

  set ev        "Event list";
      c         "fx constraints";
      evc{ev,c} "ev-c relation";
  parameter FX{c,i,j,k,w,s} "Table 9 in PLA"; 
            FXX{c,i,j,k,w,s} "a copy";  
            Fx{c}   "projection on c of FX"; 
            Fxx{c}  "a copy";
            Pen{c}  "penalty of constraint"; 
            REG{c}  "at least/at most";
            Vio{c} "violated constraints"; 
  string parameter 
    evForm{ev} "formula in Table 5"; 
    evComm{ev} "comment in Table 5"; 
    evCo1{ev}  "column1 in Table 5"; 
    evCo3{ev}  "column3 in Table 5";

  set t,t1   "Set for RR-separation (ABZ)";
  parameter Q{i,j,k} "match (i,j,k) in 2nd RR not possible";
  
  string parameter HA{i,k} "HA-Matrix";
  
  /** Copy of variables as parameters */
  binary parameter Z{i,j,k|i<>j}  "final 0/1 solution";
  binary parameter ZZ{i,j,k|i<>j} "starting solution";
  binary parameter ZzH{i,k} "copy of zH variable"; 
                   ZzA{i,k} "copy of zA variable";
  binary parameter ZZB{i,k} "copy of ZB variable";
  parameter KK; anf; mid; ende; loop; runDouble; warmStart; dummy; lo;
  set zz{i,j,k,w,s} "read-in solution as set";
  parameter solFile "is there a solution file (JJ[6])?";

  /** Model variables */
  binary variable x{i,j,k|i<>j and K} priority -1;
    zH{i,k|K}; zA{i,k|K}  "is home/away";  
    ZH{i,k|K}; ZA{i,k|K}  "is home/away break";  
    ZB{i,k|K}; ZN{i,k|K}  "is break/no-break";
  binary variable z{i,j,k,w,s|Z and kws} "final assign";

  model SecondOpt "Second Optimisation: partition a round into several days";
    set ijkws{i,j,k,w,s}:=Z and kws;
    /** Relations derived from FX for constr 24-26 and 40-42 */
    parameter FX24{c,i,k,w,s}; nz{c,ijkws[i,j,k,w,s]};
    set FX25{c,i,k,w,s};  FX26{c,i,k,w,s}; FX27{c,i,k,w,s};
        FX40{c,i,k,w}; s40{c,s}; FX41{i,j,k,w,s}; FX42{i,j,k,w,s};
    {FX[c,i,j,k,w,s]}
       if(24<=Fx<=26,FX24:=Fx-23,  
          Fx=40,(FX40:=1,s40:=1), Fx=41,FX41:=1, Fx=42,FX42:=1);
    nz{c,ijkws[i,j,k,w,s]} := FX24[c,i,k,w,s]=3 or  FX24[c,j,k,w,s]=3 or
       FX24[c,i,k,w,s]=FX24[c,j,k,w,s] and  FX24[c,i,k,w,s]>0 or
       FX24[c,i,k,w,s]=2 or  FX24[c,j,k,w,s]=1;
        
    constraint 
      A1{i,j,k|Z}: sum{ws[w,s]} z = 1 "Every match is in one (w,s)";
      AB{kijws[k,i,j,w,s]|block}: z = 1 "User blocked matches";
      A2min{kw[k,w]|minsp}: minsp - Sl(minsp,3) <= sum{i,j,s|Z} z 
        "At least minsp matches assigned to each day in a round";
      A2max{kw[k,w]|maxsp<#i/2}: sum{i,j,s|Z} z <= maxsp + Sl(6,3)
        "At most maxsp matches assigned to each day per round";
      B0{Z[i,j,k],ws[w,s]|iws}: z >= 1 - Sl(1,100)  "Preferred slot for i";
      
      C24{nz[c,i,j,k,w,s]}: z <= Sl(1,30);
  
    solve  "Solve the model"; 
    --minimize slots: sum{Z[i,j,k],ws[w,s]} s*z;  // favor slot one
    SolverStatus:=GetSolverStatus();
    GetViolations;
  end

  model ReadSolFromXLS "Read a solution from a XLS file";
    Read{k,i,j}('-:txt:'&PlanFile&'.xls,;1:Game,\t\n', k=2, i=11,j=12,kij);
    block{k,i,j|kij}:=1;
    NMO:=0;
  end;

  model Regenerate "Read a solution from SPS of a text file";
    //-- read a given solution from a file
    // (1) generate a solution from the sps
    --Read(PlanFile&'.sps');  

    // (2) re-generate a solution from a xls-file
    
    Read{k,i,j,w,s}('yyy.txt',dummy, k,i,j,w,s,zz,dummy,date1,zeit);
    //Read{k,i,j,w,s}(PlanFile&'.xls' '%1:Game,\t\n', dummy, k,i,j,w,s,zz,dummy,date1,zeit); 
    Z{i,j,k} := if(exist{ws[w,s]}zz,1);
    //z{i,j,k,w,s|zz}:=1;
    //{i,j,k,w,s|zz} (z[i,j,k,w,s]:=1);  

    // (3) re-generate a solution from a txt z-list-file
    -- Read{i,j,k}(PlanFile&'a.txt' , k,i,j,zz[i,j,k,WT,s]);
    -- z{i,j,k,w,s|zz}:=1;  Z{i,j,k} := if(exist{ws[w,s]}zz,1);
    SolverStatus:=7   "optimal!";
  end

  model Input "Read problem data from pla-file";
    jj:=[1..10];
    Read(PlanFile&'.pla,%1:Table', N,NNR,NMO,ABZ,PEN,DEB,picH,picW); //,seed,timeLimit);
    
    k := 1..NNR*(N-1);
    KK := N-1;
    //if NMO=8 then KK:=N-1+Trunc((N-1)/2); k:=1..KK; end "zweier-Aufteilung";
    if NMO=8 then KK:=N-1+Trunc((N-1)/3); k:=1..KK; end; --dreier Aufteilung: Isoz
    Read('%1;1', {bc}BC);
    Read('%1;2', {k}RX);
    Read{jj}('%1;3', JJ=2);
    JJ[3] := PlanFile&'.jpg' "on server set this" ;   ////// for Server redefined
      
    {k|k<=KK} (RX[k+N-1]:=RX, RX[k]:=k), 
              {k|k>2*KK} (RX[k]:=RX[k-2*KK]+2*KK);
    Read{i}('%2,\t\n,\'', i,na,name,location,dummy,strength,group,dummy,dummy,Islot); 
    Read{i}('%3', i,name, {j}dist);
    Read{w}('%4,\t\n', w,W,dummy,minsp,maxsp,Zeit);
    Read{ev}('%5,\t\n', ev,evCo1,dummy,evCo3,evComm,dummy,evForm);
    Read{ev,c,i,j,k,w,s}('%9',ev,c,i,j,k,w,s,FX);
    Read{ev,c}('%9', ev,c,evc,dummy,dummy,dummy,dummy,dummy,Fx,Pen,REG,evForm);
    Read{k,w,s}('%6', k,w,kw,date1);
    Read{k,i,j,w,s}('%7', dummy,k,i,j,w,s,kij,kijws,date2, block,comm3);
    Read{i,j,k}('%10',i,j,k,ZZ);
    --Read{i,j,k}('sol.txt' , i,j,k,ZZ); //for debugging
    warmStart := sum{i,j,k|ZZ} 1;
    solFile:=JJ[6]<>'';  //there is a solution file
    if solFile then
      Read{k,i,j,w,s}(JJ[6]&',;1',dummy, k,i,j,w,s,zz,dummy,date1,zeit);
      PEN:=1000;
    end;
    {ev} (evCo3 := if(evCo3='*',' in all rounds',' in round(s) '&evCo3),
          evCo1 := evCo1&' ', 
          evCo1 := if(evCo1='* ','For all teams: ',Strsub(evCo1,1,Strpos(' ',evCo1))),
          evComm := evCo1&': '&evComm&evCo3);
    {w} Split(Zeit,',',{s}zeit);
    string parameter w0; s0;
    {i} if(Islot<>'',(Split(Islot,':',w0,s0), iws[i,Strfloat(w0),Strfloat(s0)]:=1));
    ws{w,s}:=zeit;
    kws{k,w,s}:=kw and ws;
    //KW{w}:=sum{k|kw} 1;
    SolverObj:=0;
    
    showI{i}:= name[i]<>'dummy';
    N1:=sum{i} showI - 1;
    //DUM:= if(na[#i]='dum',#i);
    {i} if(name[i]='dummy',DUM:=i);  //2024/06/13 new modification
    if ABZ>0 then t:=1..ABZ; end;

    /** For lizenz reason this file is written om the server */
    Write('+:league.txt','%d\t%s\t%s\t%s\t%s\t%d\t%d\t%d\t%s\n',
      1000*Now(),GetParamS(16),GetParamS(15),DateToStr(Now()),JJ[3],N,NNR,NMO,name[1]);
    FXX{FX[c,i,j,k,w,s]}:=FX; 
    Fxx{c}:=Fx;
  end

  model Output "Generate final output results: XLS, RES, PDF ...";
    integer parameter X{i,k}; d;
    integer XX{i,k}; YY{i,k}; pp{k}:=1;
    {i,k} (d:=0, 
           d:=argmax{j|Z} Z, 
           if(d>0,(X:=d,X[d,k]:=-i)));
    {i,k} (if(X>0,(XX[pp[k],k]:=X, YY[pp[k],k]:=i, pp[k]:=pp[k]+1))); 

    string parameter HAR{i,k}:=if(X[i,k]>0,'H','A') "Resulting Home-Away Pattern";;
      HAR{i,k}:=if(Abs(X)=DUM,' ',HAR);  //do not show byes
    parameter sHH{i}:= sum{k|HAR[i,k-1]=HAR and HAR='H'} 1 "total HH-breaks";
    parameter sAA{i}:= sum{k|HAR[i,k-1]=HAR and HAR='A'} 1 "total AA-breaks";

    // carry over value
    --parameter CarryOver{i,j}:=sum{j1,k|k<#k and (Z[j1,i,k] or Z[i,j1,k]) and (Z[j1,j,k+1] or Z[j,j1,k+1])} 1;
    --parameter COE:=sum{i,j} CarryOver^2;
    
    parameter DIST{i,k}:= if(k<#k,d:=k+1,d:=1), 
                          if(X[i,k]<0 and X[i,d]<0,dist[-X[i,k],-X[i,d]],
                             X[i,k]<0 and X[i,d]>0,dist[-X[i,k],i],
                             X[i,k]>0 and X[i,d]<0,dist[i,-X[i,d]]);

    //parameter XXX{i,j,k,k1|i<j and k<k1}:=X[i,k]=-X[j,k1] and X[j,k]=-X[i,k1] and (X[i,k]<0 and X[j,k]<0 or X[i,k]>0 and X[j,k]>0);
    {i,j,k,w,s| z} (date2 := if(date2,date2,date1));
    
    /** Output RES-file */
    Write(PlanFile&'.res','Constraint violations --Total penalty:\t%d\n',SolverObj) "Violations";
    if sum{c}Vio<0.1 then Write('   no violated constraint\n'); end;
    Write{evc[ev,c]|Vio>0.1}('  %3s\t%3s\t%4dx%4d\t%-25s\t%-30s\n', ev,c,Vio,Pen,evForm,evComm);
    
    Write('\n%-28s%3d \n', 'Home-Away Pattern', {k}k) "H-A Pattern";  
    Write{i}('%-28s %3s \n', name, {k}Format('%1s%1s%1s', if(HAR[i,k-1]=HAR,'-',' '),HAR, if(HAR[i,k+1]=HAR,'-',' ')));  

    Write('\nTotal Breaks: %4d + %4d = %4d\n', sum {i}sHH, sum {i}sAA, sum {i}(sHH+sAA)) "Total breaaks";  
    Write{i}('%4s = %-28s H-H , A-A , Total Breaks: %4d + %4d = %4d \n', na,name,sHH,sAA,sHH+sAA));  
    
    Write('\n%-28s %-15s %4s\n', 'Home Matches', {w}Format('%2s %2s ', W ,{s|ws} s) ,'TOTAL') "Home matches";  
    Write{i}('  %-26s %-15s %4d\n', name, {w}Format('%2D %2D ', sum{j,k,s} z ,{s|ws} sum{j,k} z), sum{j,k,w,s} z);    
    Write('  %-26s %-15s %4d\n', 'TOTAL', {w}Format('%2D %2D ', sum {i,j,k,s}z,{s|ws} sum {i,j,k}z) , sum{i,j,k,w,s} z); 
 
    Write('\n%-28s %-15s %4s\n', 'Matches/Round/Weekday', {w}Format('%2s %2s ', W ,{s|ws} s) ,'TOTAL') "Matches/Round/Weekday";  
    Write{k}('  %-26s %-15s %4d\n', k, {w}Format('%2D %2D ', sum{i,j,s} z ,{s|ws} sum{i,j} z), sum{i,j,w,s} z);    
    
    Write('\nComplete Schedule\n') "Complete Schedule";
    Write('       | %6s\n', {k}k);
    Write{i}('%6s | %6s\n', na, {k} if (X<0,'*'&na[Abs(X)],na[Abs(X)]));
    Write('\n     %-9s\n', {k}k);
    Write{i|i<=#i/2}(' %9s \n', {k}Format('%4s:%-4s',na[YY],na[XX]));
    
    set fx24{i,kws[k,w,s]}:=exist{c} (exist{ws[w1,s1]} SecondOpt.FX24[c,i,k,w1,s1] and ~SecondOpt.FX24[c,i,k,w,s]);
    set fx25{i,kws[k,w,s]}:=exist{c} SecondOpt.FX25[c,i,k,w,s];
    set fx41{i,kws[k,w,s]}:=exist{ws[w1,s1]} SecondOpt.FX41[i,X,k,w1,s1] and ~SecondOpt.FX41[i,X,k,w,s];
    set fx42{i,kws[k,w,s]}:=SecondOpt.FX42[i,X,k,w,s];
    set Ww{i,j,kws[k,w,s]|Z and ~z and ~(fx24[i,k,w,s] or fx24[j,k,w,s]) and 
                                       ~(fx25[i,k,w,s] or fx25[j,k,w,s]) and 
					         ~fx41 and ~(fx42[i,k,w,s] or fx42[j,k,w,s])} := 1;
    {k}( Write('\nRound %d\n', k),
         Write{i|X>0}(' %4s -- %4s \t (%tF,%s) \t poss. slots are: %s\n', na,na[X] , date2[k,i,X],
           {ws[w,s]|z[i,X,k,w,s]} Format('%s,%d', W[w] , s) , 
           {ws[w,s]|Ww[i,X,k,w,s]} Format('(%s,%d)', W[w], s) ) ) "Round-List";

    Write('\nAll matches in different rounds\n') "Matches in rounds";
    parameter diffR{i,j}:=Abs(argmax{k} Z[i,j,k] - argmax{k} Z[j,i,k]);
    Write{i,j|i<>j}('  %4s -- %4s   Rounds: %4s    diff: %2d\n', na[i],na[j], {k|Z[i,j,k] or Z[j,i,k]}Format('%3s%1s', k, if (Z[i,j,k],' ','*')),diffR);   

    Write('\nRound Trips possible\n') "Round-trips";
    parameter X6{c}:=Fx=6 and Vio=0;
    Write{FX[c,i,j,k,w,s] | X6 and Z[i,REG,k] and Z[j,REG,k+1]}('%3d: %s goes to %s then to %s in round %d\n', c,na[REG],na[i],na[j],k);
    
//    Write('Slot number for each game\n');
//    Write('    %2s \n', {i} i);
//    Write{i}(' %2 %2s \n', i, );    

    /** Output XLS-file */
    integer parameter nn:=0; 
    Write('txt:'&PlanFile&'.xls','Game\tRound\tDay\tDate\tTime\tHome\tAway\tVenue\tFree-TV\tComm2\tComm3\tHome1\tAway1\tDay1\tSlot\tDate1\tc1\tc2\tblocked\tI\tJ\tPlay Slot\tOther poss. play slots\n');
    Write{k,i,j,w,s|z}('%d\t%d\t%s\t%tF\t%s\t%s\t%s\t%s \t%s\t%s\t%s \t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%s\t%s\n', 
      nn:=nn+1,k,W,date2,zeit,name[i],name[j],location[i], ' ',' ',comm3,i,j,w,s,date2,1,1,block,na[i],na[j],
      {ws[w1,s1]|z[i,X,k,w1,s1]} Format('%s,%d', W[w1] , s1) , {ws[w1,s1]|Ww[i,X,k,w1,s1]} Format('(%s,%d)', W[w1], s1) )  );

    /** Output CSV-File */
    nn:=0; 
    Write('txt:'&PlanFile&'.csv','Game,Round,Day,Date,Time,Home,Away,Venue,Free-TV,Comm2,Comm3,Home1,Away1,Day1,Slot,Date1,c1,c2,blocked,I,J,"Play Slot","Other poss. play slots"\n');
    Write{k,i,j,w,s|z}('%d,%d,%s,%tF,%s,"%s","%s","%s" ,"%s","%s","%s" ,%d,%d,%d,%d,%d,%d,%d,%d,%s,%s,%s,%s\n', 
      nn:=nn+1,k,W,date2,zeit,name[i],name[j],location[i], ' ',' ',comm3,i,j,w,s,date2,1,1,block,na[i],na[j],
      {ws[w1,s1]|z[i,X,k,w1,s1]} Format('%s,%d', W[w1] , s1) , {ws[w1,s1]|Ww[i,X,k,w1,s1]} Format('(%s,%d)', W[w1], s1) )  );

    /** Output SPS-file and sol file */
    --Write(PlanFile&'.sps');
    --Write{i,j,k|Z}('sol.txt','%d\t%d\t%d\n',i,j,k,Z);
    
    /** Output Report (pdf/rtf)-files */
    Write('&:tmp.db,league0,*' , 'head'='Plan: '&JJ[5],'titel'=JJ[2],'pic'=JJ[3],'subtitel'=JJ[4]);
    Write('*:tmp.db,tableH1' , 'debug'=DEB,'titel'=JJ[2],'pic'=JJ[3],'picWidth'=picW,'subtitel'=JJ[4],'vo'='www.virtual-optima.com');
    Write{k,i,j,w,s|z and showI[i] and showI[j]}
	     ('*:tmp.db,league1,*,'&PlanFile&'.pdf' , 'rr'=k+0,'dd'=date1,'round'='Round '&k,'da'=date1,'z'=zeit,'Heim'=name[i],'Away'=name[j],'res'='_:_','h'=HA,'h1'=HA[j,k]);

    Write('&:tmp.db,league0,*' , 'head'='Plan: '&JJ[5],'titel'=JJ[2],'pic'=JJ[3],'subtitel'=JJ[4]);
    Write('*:tmp.db,tableH1' , 'debug'=DEB,'titel'=JJ[2],'pic'=JJ[3],'picWidth'=picW,'subtitel'=JJ[4],'vo'='www.virtual-optima.com');
    Write{k,i,j,w,s|z and showI[i] and showI[j]}
	     ('*:tmp.db,league1,*,'&PlanFile&'.rtf' , 'rr'=k+0,'dd'=date1,'round'='Round '&k,'da'=date1,'z'=zeit,'Heim'=name[i],'Away'=name[j],'res'='_:_','h'=HA,'h1'=HA[k,j]);

//    Write('&:tmp.db,league2,*' , 'head'='Documentation','titel'=JJ[5],'subtitel'=Now());
//    Write('*:tmp.db,tableH1' , 'debug'=DEB,'titel'=JJ[2],'pic'=JJ[3],'picWidth'=picW,'subtitel'=JJ[4],'vo'='www.virtual-optima.com');
//    Write{i}('*:tmp.db,tableH2' , 'rr'=i+0, 'na'=na,'name'=name,'loc'=loc);
//    Write{i}('*:tmp.db,tableH3' , 'rr'=i+0, 'na'=na,'name'=name,'loc'=loc);
//    Write{('*:tmp.db,league3,*,'&PlanFile&'.pdf'
  end 
  
  model Wr "This sub-model is not used";
    integer parameter X{i,k}; d;
    integer XX{i,k}; YY{i,k}; pp{k}:=1;
    {i,k} (d:=0, d:=argmax{j|Z} Z, if(d>0,(X:=d,X[d,k]:=-i)));
    {i,k} (if(X>0,(XX[pp[k],k]:=X, YY[pp[k],k]:=i, pp[k]:=pp[k]+1))); 
    Write('OBJ=%d\n',sum{c} Pen*Vio);
    Write('       | %6s\n', {k}k);
    Write{i}('%6s | %6s\n', na, {k} if (X<0,'*'&na[Abs(X)],na[Abs(X)]));
  end


/* end of leagueA.lpl */

