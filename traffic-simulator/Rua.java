/**
 * Representa uma rua. Para os nossos fins, a rua é apenas
 * um segmento que tem início e fim.
 *
 * Fernando Bevilacqua <fernando.bevilacqua@uffs.edu.br>
 */

import javafx.geometry.Point2D;
import java.util.Vector;

public class Rua
{

	public Point2D inicio; 					// Onde a rua inicia no espaco.
	public Point2D fim;						// Onde a rua termina
	public Point2D semaforo;				// Posicao do semaforo na rua
	public boolean verde;					// Se o semaforo dessa rua estah verde
	public long contadorVerde;				// Contagem de tempo quando o semaforo estah verde.
	public long segundosFicaVerde;			// Quanto tempo, em segundos, o semaforo deve ficar verde.
	public int carrosPorMinuto;				// Quantos carros por minuto passam pelo semaforo, em media.
	public Vector<Entidade> veiculos;		// Lista de veiculos existentes nessa rua.
	public long contadorSpawn;				// Contador usado para criar os carros conforme a taxa de fluxo informada (carrosPorMinuto)
	public long carrosAdicionados;			// Quantidade de carros que foram adicionados a essa rua durante a simulacao
	public long carrosPassaramSemaforo;		// Quantidade de carros que passaram pelo semaforo e chegaram no fim da rua
	public Vector<Entidade> remover;

	public Rua(Point2D inicio, Point2D fim, int carrosPorMinuto, long segundosFicaVerde) {
		this.inicio 				= inicio;
		this.fim					= fim;
		this.semaforo				= inicio.add(fim).multiply(0.5); // assumimos que o semaforo fica na metade do caminho
		this.veiculos				= new Vector<Entidade>();
		this.carrosPorMinuto		= carrosPorMinuto;
		this.contadorSpawn			= 0;
		this.verde					= false;
		this.segundosFicaVerde		= segundosFicaVerde * 1000;
		this.contadorVerde			= 0;
		this.carrosAdicionados		= 0;
		this.carrosPassaramSemaforo	= 0;
		this.remover				= new Vector<Entidade>();
	}

	public void update(long delta) {
		updateLogicaSemaforo(delta);
		updateLogicaAdicionarCarroNovo(delta);

		for(Entidade v : veiculos) {
			v.update(delta, this);

			// A unidade chegou ateh o final da rua?
			if(v.posicao.distance(fim) <= Constants.ENTIDADE_DISTANCIA_FIM_REMOVER) {
				// Sim! Vamos marcar ela para remover da simulacao.
				remover.add(v);
			}
		}

		removeVeiculosMarcadosParaRemocao();
	}

	private void removeVeiculosMarcadosParaRemocao() {
		int i;

		for(i = 0; i < remover.size(); i++) {
			carrosPassaramSemaforo++;
			veiculos.remove(remover.get(i));
		}

		// Limpa a lista de elementos a serem removidos.
		remover.removeAllElements();
	}

	private void updateLogicaSemaforo(long delta) {
		// O semaforo estah verde?
		if(verde) {
			// Sim. Entao vamos contar o tempo que ficamos verdes
			contadorVerde += delta;

			if(contadorVerde >= segundosFicaVerde) {
				verde = false;
			}
		} else {
			contadorVerde = 0;
		}
	}

	private void updateLogicaAdicionarCarroNovo(long delta) {
		contadorSpawn += delta;

		if(carrosPorMinuto > 0 && (contadorSpawn >= (60000 / carrosPorMinuto))) {
			contadorSpawn = 0;
			adicionaCarroNovo();
		}
	}

	private void adicionaCarroNovo() {
		Entidade v = new Entidade();

		v.posicao = new Point2D(inicio.getX(), inicio.getY());
		v.velocidade = fim.subtract(inicio).normalize().multiply(Constants.ENTIDADE_VELOCIDADE);

		veiculos.add(v);
		carrosAdicionados++;
	}
}
