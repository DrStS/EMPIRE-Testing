
classdef interfaceJacobian < handle
    % This class integrates a 1D linear truss element in time
    properties (GetAccess = 'public', SetAccess='public')      
      epsilonResidual;
      interfaceDOFs;
      globalResidualVector_k= [0;0];
      globalResidualVector_k_m_1= [0;0];
      globalCorrectionVector;
      globalJacobianMatrix;
      omega_k_m_1    = 1.0;
      omega_k        = 1.0;
    end
    methods (Access=public)
        %Constructor
       function obj=interfaceJacobian(epsilonResidual_, interfaceDOFs_)
           obj.epsilonResidual=epsilonResidual_;
           obj.interfaceDOFs  =interfaceDOFs_;
           obj.globalJacobianMatrix = eye(obj.interfaceDOFs);
       end
       
    
       %%   GLOBAL FUNCTIONS
       function [residual,convergenceFlag]=sendCheckResidual(obj,systemIdentifier)          
        residual=obj.globalResidualVector_k(systemIdentifier,1);
        disp(['Current residual is: ', num2str(residual)]);
        if abs(residual) < obj.epsilonResidual
            convergenceFlag=true;
        else
            convergenceFlag=false;
        end
       end
       
       function receiveResidual(obj,interfaceQuantity,interfaceQuantityOld,systemIdentifier)  
            obj.globalResidualVector_k_m_1(systemIdentifier,1)=obj.globalResidualVector_k(systemIdentifier,1);
            obj.globalResidualVector_k(systemIdentifier,1)=interfaceQuantity-interfaceQuantityOld;
       end
       
       function receiveJacobian(obj, interfaceJacobian, systemIdentifier)  
           if (systemIdentifier == 1) 
            obj.globalJacobianMatrix(2,1)=-interfaceJacobian;
           elseif (systemIdentifier == 2) 
            obj.globalJacobianMatrix(1,2)=-interfaceJacobian;
           elseif (systemIdentifier == 3) 
            obj.globalJacobianMatrix(1,2)=obj.globalJacobianMatrix(1,2)-interfaceJacobian;
           end
       end
       
       function doSolveCorrection(obj)  
           symJacMatrix= obj.globalJacobianMatrix'*obj.globalJacobianMatrix;
           %disp('Eigenvalues of sym globalJacobianMatrix');
           %disp(eig(symJacMatrix));
           symResVec= obj.globalJacobianMatrix'*obj.globalResidualVector_k;
           
           obj.globalCorrectionVector=symJacMatrix\symResVec;
       end
       
       function [correction]=getCorrection(obj,systemIdentifier)
           correction=obj.globalCorrectionVector(systemIdentifier,1);
       end     
       
       function [relax]=computeAitkenRelaxation(obj)
           
           if(abs(obj.globalResidualVector_k_m_1(2)-obj.globalResidualVector_k(2))>1e-6)
           obj.omega_k=obj.omega_k_m_1+obj.omega_k_m_1*((obj.globalResidualVector_k(2))/(obj.globalResidualVector_k_m_1(2)-obj.globalResidualVector_k(2)));
           %obj.omega_k=obj.omega_k_m_1+obj.omega_k_m_1*((obj.globalResidualVector_k(2))/(obj.globalResidualVector_k_m_1(2)-obj.globalResidualVector_k(2)));
           obj.omega_k_m_1=obj.omega_k;
           end
           if(obj.omega_k==0)
            obj.omega_k_m_1=0.1;
            obj.omega_k=0.1;
           end
                           
           relax=obj.omega_k;
       end   
       
       function resetAitkenRelaxation(obj)
          obj.globalResidualVector_k= [0;0];
          obj.globalResidualVector_k_m_1= [0;0];
          obj.omega_k_m_1    = 0.1;
          obj.omega_k        = 0.1;
       end 
       
       
    %end methods public
    end
    methods (Access=private)

    %end methods private
    end
%end classdef
end







