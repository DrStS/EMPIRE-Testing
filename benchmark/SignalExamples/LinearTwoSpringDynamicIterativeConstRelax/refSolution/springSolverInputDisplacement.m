classdef springSolverInputDisplacement < springSolver
    % This class integrates a 1D linear truss element in time
    properties (GetAccess = 'public', SetAccess='public')   
    end
    methods (Access=public)
        %Constructor
       function obj=springSolverInputDisplacement(m_,k_,h_,tend_,omegacoupled_)
            disp('Generating Object springSolverInputDisplacement');
            obj = obj@springSolver(m_,k_,h_,tend_,omegacoupled_); 
       end
    
    %% DO INTEGRATION USE BE INTEGRATOR   
    function [interfaceForce]=doSolve(obj,interfaceDisplacement)          
           integratorCoeff = (obj.m / (obj.h*obj.h)) + obj.k; 
           obj.u_n_p_1=interfaceDisplacement;
           interfaceForce=-(integratorCoeff*obj.u_n_p_1-((obj.m/(obj.h*obj.h))*(2*obj.u_n-obj.u_n_m_1)));
    end
    
    function [jacVector]=getInterfaceJacobian(obj)
           %Return the Jacobian
           jacVector=(-obj.m-obj.k*obj.h*obj.h)/(obj.h*obj.h);
    end 
    
    function [systemIdentifier]=getSystemIdentifier(obj)
           systemIdentifier=2;
    end 
    
    %end methods public
    end
    %%   PROTECTED FUNCTIONS
    methods (Access=protected)  
    %end methods private
    end
%end classdef
end







