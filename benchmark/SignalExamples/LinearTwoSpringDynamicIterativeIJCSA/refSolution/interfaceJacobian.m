
classdef interfaceJacobian < handle
    % This class integrates a 1D linear truss element in time
    properties (GetAccess = 'public', SetAccess='public')      
      epsilonResidual;
      globalResidualVector= [0;0];
      globalCorrectionVector;
      globalJacobianMatrix= eye(2);
    end
    methods (Access=public)
        %Constructor
       function obj=interfaceJacobian(epsilonResidual_)
           obj.epsilonResidual=epsilonResidual_;
           obj.globalJacobianMatrix = eye(2);
       end
       
    
       %%   GLOBAL FUNCTIONS
       function [residual,convergenceFlag]=sendCheckResidual(obj,systemIdentifier)          
        residual=obj.globalResidualVector(systemIdentifier,1);
        disp(['Current residual is: ', num2str(residual)]);
        if abs(residual) < obj.epsilonResidual
            convergenceFlag=true;
        else
            convergenceFlag=false;
        end
       end
       
       function receiveResidual(obj,interfaceQuantity,interfaceQuantityOld,systemIdentifier)  
            obj.globalResidualVector(systemIdentifier,1)=-interfaceQuantity+interfaceQuantityOld;
       end
       
       function receiveJacobian(obj, interfaceJacobian, systemIdentifier)  
           if (systemIdentifier == 1) 
            obj.globalJacobianMatrix(2,1)=-interfaceJacobian;
           elseif (systemIdentifier == 2) 
            obj.globalJacobianMatrix(1,2)=-interfaceJacobian;
           end
       end
       
       function doSolveCorrection(obj)  
           symJacMatrix= obj.globalJacobianMatrix'*obj.globalJacobianMatrix;
           symResVec= obj.globalJacobianMatrix'*obj.globalResidualVector;
           
           obj.globalCorrectionVector=symJacMatrix\-symResVec;
       end
       
       function [correction]=getCorrection(obj,systemIdentifier)
           correction=obj.globalCorrectionVector(systemIdentifier,1);
       end      
       
    %end methods public
    end
    methods (Access=private)

    %end methods private
    end
%end classdef
end







