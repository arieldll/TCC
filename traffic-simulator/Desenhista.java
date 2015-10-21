/**
 * Desenha os elementos na tela.
 * Fernando Bevilacqua <fernando.bevilacqua@uffs.edu.br>
 */

import java.awt.*;
import java.io.*;
import javax.imageio.*;
import javax.swing.JFrame;

import java.awt.event.KeyListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.geom.*;
import java.awt.image.BufferStrategy;
import java.awt.image.BufferedImage;
import java.util.Calendar;
import java.util.HashMap;

public class Desenhista extends JFrame
{
	private Simulador simulador;
	private Image imgBackground;
	private Image imgSprites;

	public Desenhista() {
		final Frame a = this;

		// Need to add this to handle window closing events with the awt Frame.
		addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent arg0) {
				a.setVisible(false);
				a.dispose();
				System.exit(0);
			}
		});

		if(Constants.TELA_USAR_BARRA_TOPO) {
			//setLocationRelativeTo(null); // centraliza a janela
			setUndecorated(false);
		} else {
			setUndecorated(true);
		}

		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(Constants.LARGURA_TELA, Constants.ALTURA_TELA);
		setVisible(true);

		createBufferStrategy(2);
	}

	public void init(Simulador s, KeyListener k) {
		simulador = s;
		addKeyListener(k);
	}

	public void render() {
		// From: http://gpwiki.org/index.php/Java:Tutorials:Double_Buffering
		// Thanks!

		BufferStrategy bf = this.getBufferStrategy();
		Graphics g = null;

		try {
			g = bf.getDrawGraphics();
			renderiza(g);

		} finally {
			// It is best to dispose() a Graphics object when done with it.
			g.dispose();
		}

		// Shows the contents of the backbuffer on the screen.
		bf.show();

        //Tell the System to do the Drawing now, otherwise it can take a few extra ms until
        //Drawing is done which looks very jerky
        Toolkit.getDefaultToolkit().sync();
	}

	public void terminate() {
		WindowEvent wev = new WindowEvent(this, WindowEvent.WINDOW_CLOSING);
		Toolkit.getDefaultToolkit().getSystemEventQueue().postEvent(wev);
	}

	private void renderiza(Graphics g) {
		g.clearRect(0, 0, Constants.LARGURA_TELA, Constants.ALTURA_TELA);

		for(Rua r : simulador.getRuas()) {
			desenhaRua(g, r);
			desenhaCarros(g, r);
		}
	}

	private void desenhaCarros(Graphics g, Rua r) {
		g.setColor(Color.BLUE);

		for(Entidade v : r.veiculos) {
			g.fillRect((int)v.posicao.getX() - 5, (int)v.posicao.getY() - 5, 10, 10);
		}
	}

	private void desenhaRua(Graphics g, Rua r) {
		((Graphics2D)g).setStroke(new BasicStroke(3));

		g.setColor(r.verde ? Color.GREEN : Color.RED);
		((Graphics2D)g).drawLine((int)r.inicio.getX(), (int)r.inicio.getY(), (int)r.fim.getX(), (int)r.fim.getY());
	}
}
