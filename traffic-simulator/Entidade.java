/**
 * Representa qualquer entidade que pode ocupar uma posição dentro da
 * simulacao (e.g. carro, moto, etc.)
 *
 * Fernando Bevilacqua <fernando.bevilacqua@uffs.edu.br>
 */

import javafx.geometry.Point2D;
import java.util.Vector;

public class Entidade
{
	private static int contador	= 0;

	public Point2D velocidade;
	public Point2D posicao;
	public boolean parado;
	public long esperandoParaArrancar;
	private int id;

	// Métodos públicos
	public Entidade() {
		this.velocidade 	= new Point2D(0, 0);
		this.posicao 		= new Point2D(0, 0);
		this.parado			= true;
		this.id				= Entidade.contador++;
	}

    public void update(long delta, Rua rua) {
		if(!temAlguemNaMinhaFrente(rua) && !devoPararNoSemaforo(rua)) {
			move(delta);
		} else {
			para();
		}
    }

	private void para() {
		parado = true;
	}

	private void move(long delta) {
		// Para evitar que todos os carros arranquem ao mesmo tempo, o inicio do movimento
		// eh sempre atrasado por algum tempo (definido aleatoriamente).

		if(parado) {
			parado = false;
			esperandoParaArrancar = (long)(Simulador.random.nextDouble() * Constants.ENTIDADE_TEMPO_ESPERA_ARRANQUE * 1000);
		}

		if(esperandoParaArrancar > 0) {
			esperandoParaArrancar -= delta;

		} else {
			posicao = posicao.add(velocidade);
		}
	}

	private boolean acabeiDePassarPeloSemaforo(Rua rua) {
		return rua.inicio.distance(posicao) >= rua.fim.distance(posicao);
	}

	private double minhaDistanciaAtehSemaforo(Rua rua) {
		return posicao.distance(rua.semaforo);
	}

	private boolean devoPararNoSemaforo(Rua rua) {
		return minhaDistanciaAtehSemaforo(rua) <= Constants.ENTIDADE_DISTANCIA_PARA_SEMAFORO && rua.verde == false && !acabeiDePassarPeloSemaforo(rua);
	}

	private boolean temAlguemNaMinhaFrente(Rua rua) {
		for(Entidade v : rua.veiculos) {
			// Soh olhamos para os veiculos que estao na nossa frente. Para
			// isso, iteramos ateh que sejamos encontrados na fila.
			if(v == this) {
				break;
			}

			if(v.posicao.distance(posicao) <= 20) {
				return true;
			}
		}

		return false;
	}
}
