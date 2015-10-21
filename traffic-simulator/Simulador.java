/**
 * Simula o trafego de carros em um cruzamento com varias
 * vias e sinaleiras.
 *
 * Fernando Bevilacqua <fernando.bevilacqua@uffs.edu.br>
 */

import java.awt.event.KeyEvent;
import java.util.*;
import javafx.geometry.Point2D;

public class Simulador implements Runnable {
	private Vector<Rua> ruas;
	private Desenhista desenhista;
	private Teclado teclado;
	private long ultimoUpdate;
	private long horaInicio;
	private boolean ativa;
	private long intervaloUpdate;
	private boolean debug;
	private int semaforoVerdeDaVez;

	public static Random random;

	public Simulador() {
		// Inicializamos as coisas da arena (agentes, energia, etc)
		criaAmbiente();
		adicionaRuas();

		// O primeiro semaforo a ficar verde eh o da rua zero.
		semaforoVerdeDaVez = 0;
		ruas.get(semaforoVerdeDaVez).verde = true;

		// Depois que tudo estiver configurado, arrumamos a tela
		// e iniciamos a aplicação
		initTela();
	}

	private void adicionaRuas(){
		ruas.add(new Rua( 
			new Point2D(0, Constants.ALTURA_TELA / 2 - 10),								// Onde a rua inicia
			new Point2D(Constants.LARGURA_TELA, Constants.ALTURA_TELA / 2 - 10),		// Onde a rua termina
			10, /*4.86 arredondado*/																			// Fluxo de carros por minuto
			20																			// Tempo que o semaforo fica verde
		)); //Zona 1

		ruas.add(new Rua(
			new Point2D(Constants.LARGURA_TELA, Constants.ALTURA_TELA / 2 + 10),
			new Point2D(0, Constants.ALTURA_TELA / 2 + 10),
			10, /*5.09 arredondado*/
			20
		)); //Zona 3

		ruas.add(new Rua(
			new Point2D(Constants.LARGURA_TELA / 2 - 10, 0),
			new Point2D(Constants.LARGURA_TELA / 2 - 10, Constants.ALTURA_TELA),
			20, /*9.92*/
			35
		)); //Zona 2

		/*ruas.add(new Rua(
			new Point2D(Constants.LARGURA_TELA / 2 + 10, Constants.ALTURA_TELA),
			new Point2D(Constants.LARGURA_TELA / 2 + 10, 0),
			20,
			2
		)); 
		
		/*ruas.add(new Rua(
			new Point2D(0, Constants.ALTURA_TELA / 2 - 10),								// Onde a rua inicia
			new Point2D(Constants.LARGURA_TELA, Constants.ALTURA_TELA / 2 - 10),		// Onde a rua termina
			50,																			// Fluxo de carros por minuto
			10																			// Tempo que o semaforo fica verde
		));

		ruas.add(new Rua(
			new Point2D(Constants.LARGURA_TELA, Constants.ALTURA_TELA / 2 + 10),
			new Point2D(0, Constants.ALTURA_TELA / 2 + 10),
			40,
			5
		));

		ruas.add(new Rua(
			new Point2D(Constants.LARGURA_TELA / 2 - 10, 0),
			new Point2D(Constants.LARGURA_TELA / 2 - 10, Constants.ALTURA_TELA),
			20,
			3
		));

		ruas.add(new Rua(
			new Point2D(Constants.LARGURA_TELA / 2 + 10, Constants.ALTURA_TELA),
			new Point2D(Constants.LARGURA_TELA / 2 + 10, 0),
			20,
			2
		)); */
	}

	private void atualizaControleSemaforoAberto() {
		// O semaforo da vez ainda estah verde?
		if(ruas.get(semaforoVerdeDaVez).verde == false) {
			// Nao, o semaforo verde da vez estah vermelho agora.
			// Hora de avisar que o proximo na lista deve ficar verde
			semaforoVerdeDaVez++;

			if(semaforoVerdeDaVez >= ruas.size()) {
				semaforoVerdeDaVez = 0;
			}

			ruas.get(semaforoVerdeDaVez).verde = true;
		}
	}

	private void imprimeEstatisticas() {
		int i = 0;

		System.out.println("Tempo de simulacao (segundos): " + (System.currentTimeMillis() - getTimestampInicio()) / 1000);

		for(Rua r : ruas) {
			System.out.println("Rua " + i + ": adicionados = " + r.carrosAdicionados +", finalizaram = " + r.carrosPassaramSemaforo + " (" + ((double)r.carrosPassaramSemaforo/r.carrosAdicionados)*100.0 + "%)");
			i++;
		}
	}

	private void initTela() {
		this.desenhista = new Desenhista();
		this.desenhista.init(this, teclado);
	}

	private void criaAmbiente() {
		ativa			= true;
		ultimoUpdate 	= 0;
		intervaloUpdate	= Constants.INTERVALO_UPDATE;
		teclado			= new Teclado();
		ruas 			= new Vector<Rua>();

		// Provedor global de valores aleatorios
		Simulador.random = new Random(Constants.SEED_RANDOM);
	}

	public Vector<Rua> getRuas() {
		return this.ruas;
	}

	public Desenhista getDesenhista() {
		return this.desenhista;
	}

	public void run() {
		long agora;
		long delta;

		horaInicio = System.currentTimeMillis();

		while (ativa) {
			agora = System.currentTimeMillis();
			delta = agora - ultimoUpdate;

			if (delta >= intervaloUpdate) {
				update(delta);
				ultimoUpdate = agora;
			}

			processaTeclado();
			desenhista.render();
		}

		imprimeEstatisticas();
		desenhista.terminate();
	}

	public void termina() {
		ativa = false;
	}

	private void processaTeclado() {
		if(teclado.isKeyDown(KeyEvent.VK_UP)) {
			intervaloUpdate -= Constants.INTERVALO_UPDATE_INCREMENTO;

		} else if(teclado.isKeyDown(KeyEvent.VK_DOWN)) {
			intervaloUpdate += Constants.INTERVALO_UPDATE_INCREMENTO;

		} else if(teclado.isKeyDown(KeyEvent.VK_RIGHT) || teclado.isKeyDown(KeyEvent.VK_LEFT)) {
			intervaloUpdate = Constants.INTERVALO_UPDATE;
		}

		if(teclado.isKeyDown(KeyEvent.VK_Q) || teclado.isKeyDown(KeyEvent.VK_ESCAPE)) {
			termina();
		}

		if(teclado.isKeyDown(KeyEvent.VK_D)) {
			debug = true;
		} else {
			debug = false;
		}
	}

	private void update(long delta) {
		for (Rua r : ruas) {
			r.update(delta);
		}

		atualizaControleSemaforoAberto();
	}

	public long getTimestampInicio() {
		return horaInicio;
	}

	public boolean isDebug() {
		return debug;
	}
}
