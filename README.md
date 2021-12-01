 Lab 6: 

	O tempo de execução do loop foi medido através do uso de um cronômetro,
	em 10 milhões de execuções o tempo atingido foi de 7,5s, portanto cada execução
	leva por volta de 0,75us.
	
	6a-	Pelo time-slicing de 50ms as tarefas possuem slots de tempo iguais, e os leds
	piscam alternadamente de forma rápida.
	
	6b- Sem a utilização de preempção ou time-slice, as tarefas não são interrompidas
	e uma tarefa só ganha a execução quando a atual terminar.
	
	6c- Na preempção por prioridade, tarefas mais prioritárias podem interromper a 
	execução das com menor prioridade. Desta forma, o Led1 executa em seu período natural
	e os Leds 1 e 2 têm seu período aumentado.
	 
	6d- Com o mutex compartilhado pelos Leds 1 e 3, seus períodos de execução são aumentados,
	pois um Led só consegue o acesso ao recurso compartilhado quando o outro liberar.
	
	6e- No caso do mutex compartilhado com herança de prioridade, além dos períodos de
	execução dos Leds 1 e 3 serem alterados, como no caso 6d, o Led2 também pode ter seu
	período alterado.