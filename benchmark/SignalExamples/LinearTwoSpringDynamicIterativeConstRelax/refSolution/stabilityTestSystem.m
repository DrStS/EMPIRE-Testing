clc;
clear;
close all;
format long e;
%% Common parameters 
tend=5;                               % end time of simulation 
h=0.1;                                % time step size
iterend=6;                            % iteration max
intermin=1;                           % minimal number of iterations
%% Parameters for solvers
m=1;                                  % mass
k=1;                                  % stiffness
omegacoupled=sqrt(k/m);               % eigenfrequency of the coupled system
beta_1=0.45;                          % mass ratio
beta_2=0.5;                           % stiffness ratio           
%% Parameters for interface jacobian
eps = 1e-8;                           % interface residual allowed
%%

%% Initialization
% Initialization of the two client codes
system1 =springSolverInputForce        (beta_1*m    ,beta_2*k    ,h,tend,omegacoupled);
system2 =springSolverInputDisplacement ((1-beta_1)*m,(1-beta_2)*k,h,tend,omegacoupled);
% Initialization of the interface client
interfaceJacobian =interfaceJacobian(eps,1);
% Interface variables
interfaceForceOld =0;                 % old interface force
interfaceDisplacementOld=0;           % old interface displacement
convergenceFlag=false;                % convergence flag 
iter=0;                               % iteration count
relax=1;                              % default relaxation is 1 ==> no relaxation
residualForce=0;                      % interface residual for forces
residualDisplacement=0;               % interface residual for displacements
convergenceFactor=0;                  % convergence factor for algorithm 0  and 1 
% IO history for EMPIRE
io_history_sub2=[0];
%% Compute convergence factors and optimal relaxation parameter

%% Simulation 
%Time step loop
for i = 1:(round(tend/h))
    disp('===============================================================');
    disp('===============================================================');
    disp(['Time step: ', num2str(i), ' started!']);
    disp(['Convergence factor: ', num2str(abs(convergenceFactor))]);
    
    %Iteration loop
    while ~((iter >= iterend) || (convergenceFlag ==true && iter >= intermin))%
        disp('------------------------------------------------');
        disp(['Iteration number: ', num2str(iter+1), ' started!']);
        
        disp('Gauss-Seidel');
        %Run both clients Gauss-Seidel
        io_history_sub2=[io_history_sub2 interfaceDisplacementOld];
        interfaceForce       =system2.doSolve(interfaceDisplacementOld);
        io_history_sub2=[io_history_sub2 interfaceForce];
        interfaceDisplacement=system1.doSolve(interfaceForce);
        
        %Send residual to interfaceJacobian client
        interfaceJacobian.receiveResidual(interfaceDisplacement,interfaceDisplacementOld,system2.getSystemIdentifier());
        
        %Check residual
        [residualDisplacement, convergenceFlag]  =interfaceJacobian.sendCheckResidual(system2.getSystemIdentifier());
        
        
        %Swap residual
        %relax=interfaceJacobian.computeAitkenRelaxation();
        relax=0.1;
        interfaceDisplacementOld = interfaceDisplacementOld + relax*(residualDisplacement);
        
        %Increment iteration counter
        iter=iter+1;   
    %End iteration loop
    end
   
%NotifyEnd
system1.incStepCounter();
system2.incStepCounter();
%Reset increment iteration counter
iter=0; 
convergenceFlag =false;
interfaceJacobian.resetAitkenRelaxation();
%End time step loop
end
%%
fid=fopen('io_history_subSystem2_ref.dat','w');
fprintf(fid, '%e \n',io_history_sub2);
fclose(fid);

system1.plotDOFoverTIME();