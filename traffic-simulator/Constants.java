/**
 * Descreve uma arena para combate entre agentes. A arena contém uma lista com agentes
 * vivos e uma lista com os pontos de energia. Em cada iteração, todos os agentes vivos
 * são processados.
 *
 * Fernando Bevilacqua <fernando.bevilacqua@uffs.edu.br>
 */

import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.imageio.*;
import java.util.ArrayList;

public class Constants
{
	// Constantes para controle da tela (tamanho, resolução, etc).
	public static final int LARGURA_TELA 						= 900;
	public static final int ALTURA_TELA 						= 700;
	public static final int ALTURA_BARRA_TOPO_TELA 				= 30;
	public static final boolean TELA_USAR_BARRA_TOPO			= true;
	public static final long SEED_RANDOM						= 12345678l; // Semente para a geracao de numeros aleatorios.


	public static final double ENTIDADE_TEMPO_ESPERA_ARRANQUE	= 0.5;	// um rand entre zero e esse valor serah usado para qualquer entidade quando ela arrancar.
																		// Isso evita que todas as entidades arranquem ao mesmo tempo (como um trem)
	public static final double ENTIDADE_VELOCIDADE				= 10;	// velocidade das entidades na tela.
	public static final double ENTIDADE_DISTANCIA_PARA_SEMAFORO	= 30;	// distancia que a entidade parar antes do do semaforo quando ele estiver vermelho.
	public static final double ENTIDADE_DISTANCIA_FIM_REMOVER	= 30;	// Se a entidade ficar a essa distancia do ponto final da rua, ela eh removida da simulacao.

	// Define de quanto em quanto tempo a arena irá atualizar
	// todos os agentes
	public static final long INTERVALO_UPDATE 					= 100;
	public static final long INTERVALO_UPDATE_INCREMENTO		= 1;
}
