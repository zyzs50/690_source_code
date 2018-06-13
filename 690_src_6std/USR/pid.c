struct PID {
        unsigned int SetPoint; // 设定目标 Desired Value
        unsigned int Proportion; // 比例常数 Proportional Const
        unsigned int Integral; // 积分常数 Integral Const
        unsigned int Derivative; // 微分常数 Derivative Const
        unsigned int LastError; // Error[-1]
        unsigned int PrevError; // Error[-2]
        unsigned int SumError; // Sums of Errors
        };
		
		
void PIDInit (struct PID *pp)
{
        memset ( pp,0,sizeof(struct PID));           //全部初始化为0
}


unsigned int PIDCalc( struct PID *pp, unsigned int NextPoint )
{
        unsigned int dError,Error;
        Error = pp->SetPoint - NextPoint;          // 偏差           
        pp->SumError += Error;                     // 积分                                   
        dError = pp->LastError - pp->PrevError;    // 当前微分  
        pp->PrevError = pp->LastError;                           
        pp->LastError = Error;                                        
        return (pp->Proportion * Error             // 比例项           
        + pp->Integral * pp->SumError              // 积分项
        + pp->Derivative * dError);                // 微分项
}