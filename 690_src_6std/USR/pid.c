struct PID {
        unsigned int SetPoint; // �趨Ŀ�� Desired Value
        unsigned int Proportion; // �������� Proportional Const
        unsigned int Integral; // ���ֳ��� Integral Const
        unsigned int Derivative; // ΢�ֳ��� Derivative Const
        unsigned int LastError; // Error[-1]
        unsigned int PrevError; // Error[-2]
        unsigned int SumError; // Sums of Errors
        };
		
		
void PIDInit (struct PID *pp)
{
        memset ( pp,0,sizeof(struct PID));           //ȫ����ʼ��Ϊ0
}


unsigned int PIDCalc( struct PID *pp, unsigned int NextPoint )
{
        unsigned int dError,Error;
        Error = pp->SetPoint - NextPoint;          // ƫ��           
        pp->SumError += Error;                     // ����                                   
        dError = pp->LastError - pp->PrevError;    // ��ǰ΢��  
        pp->PrevError = pp->LastError;                           
        pp->LastError = Error;                                        
        return (pp->Proportion * Error             // ������           
        + pp->Integral * pp->SumError              // ������
        + pp->Derivative * dError);                // ΢����
}