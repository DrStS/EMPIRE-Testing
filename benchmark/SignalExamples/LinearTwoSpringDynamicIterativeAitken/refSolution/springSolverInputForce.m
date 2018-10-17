classdef springSolverInputForce < springSolver
    % This class integrates a 1D linear truss element in time
    properties (GetAccess = 'public', SetAccess='public')   
    end
    methods (Access=public)
        %Constructor
       function obj=springSolverInputForce(m_,k_,h_,tend_,omegacoupled_)
            disp('Generating Object springSolverInputForce');
            obj = obj@springSolver(m_,k_,h_,tend_,omegacoupled_); 
       end
    
    %% DO INTEGRATION USE BE INTEGRATOR   
    function [interfaceDisplacement]=doSolve(obj,interfaceForce)          
           integratorCoeff = (obj.m / (obj.h*obj.h)) + obj.k;       
           obj.u_n_p_1=((1/integratorCoeff)*(interfaceForce+(obj.m/(obj.h*obj.h))*(2*obj.u_n-obj.u_n_m_1)));
           interfaceDisplacement=obj.u_n_p_1;  
    end
    
    function [jacVector]=getInterfaceJacobian(obj)
           %Return the Jacobian
           jacVector=(obj.h*obj.h)/(obj.m+obj.k*obj.h*obj.h);
    end 
    
    function [systemIdentifier]=getSystemIdentifier(obj)
           systemIdentifier=1;
    end 
    
    %end methods public
    end
    %%   PROTECTED FUNCTIONS
    methods (Access=protected)  
    %end methods private
    end
%end classdef
end







