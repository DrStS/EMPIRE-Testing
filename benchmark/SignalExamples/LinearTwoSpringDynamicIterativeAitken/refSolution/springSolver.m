classdef springSolver < handle
    % This class integrates a 1D linear truss element in time
    properties (GetAccess = 'public', SetAccess='public')      
       m = 1.0            %mass
       k = 1.0            %stiffness
       omegacoupled = 1.0 %coupled eigenfrequency 
       tend =0.0;         %simulation end time
       step =1;           %time step counter
       h;                 %delta time 
       v0=1.0;            %inital velocity
       u_n_p_1;           %u_n+1
       u_n;               %u_n
       u_n_m_1;           %u_n-1 
       u_history;         %u_history 
       
    end
    methods (Access=public)
       %Constructor
       function obj=springSolver(m_,k_,h_,tend_,omegacoupled_)
            disp('Call constructor of abstract super-class');
            %Initialize with user input
            obj.m=m_;
            obj.k=k_;
            obj.h =h_;
            obj.tend=tend_;
            obj.omegacoupled=omegacoupled_;
            % Global Vectors  
            obj.u_n_p_1 = 0;
            obj.u_n     = 0;
            obj.u_n_m_1 = 0;
            obj.u_history = zeros(1,(round(obj.tend/obj.h))+1);
            % Inital conditions v0=1 u0=0
            obj.u_n_m_1=-obj.v0*obj.h;     
       end
       
    
    %%   PUBLIC FUNCTIONS
    function incStepCounter(obj)
           %Increment time step counter
           obj.step = obj.step+1;
           % Save state for output
           obj.u_history(:,obj.step) = obj.u_n_p_1;
           %Update vectors
           obj.u_n_m_1=obj.u_n;
           obj.u_n=obj.u_n_p_1;
    end 
    function plotDOFoverTIME(obj)
           %Time vector
           t=0:obj.h:obj.tend; 
           refSol=(1/obj.omegacoupled)*sin(obj.omegacoupled*t);
           plot(t,obj.u_history(1,:),'-+r');
           hold all;
           plot(t,refSol,'--+b');
           legend('numerical','analytical');
           title(['Numerical and analytical diplacment of over time.']);

    end      
    %end methods public
    end
    
    %%   PROTECTED FUNCTIONS
    methods (Access=protected)
    %end methods private
    end
    
    %%   ABSTRACT FUNCTIONS
    methods(Abstract = true)
       [output]=doSolve(obj,input);
    %end methods abstract
    end
    
    
%end classdef
end







