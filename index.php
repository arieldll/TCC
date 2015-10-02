<?php 
	error_reporting(E_ALL);
	ini_set('display_errors', 1);
	
	$zona_tempo["1"] = 15;
	$zona_tempo["2"] = 30;
	$zona_tempo["3"] = 15;
	
	$quant_zonas_rodadas = 3;
	$tempo_total_rodadas = 0;
	foreach($zona_tempo as $d => $k) $tempo_total_rodadas += $k;
	
	function retorna_tipo_veiculo($entrada){
		if($entrada==1) return "Leve";
		return "Pesado";
	}
	
	try{
		$conn = pg_connect("dbname=tcc-ariel host=127.0.0.1 user=postgres password=postgres");
		if(!$conn) echo "Erro ao conectar";
		$rs = pg_query($conn, "select veiculo as veiculo_fim 
		from (
			select *, 
			(select zona from veiculos_passagem where veiculo = vp.veiculo + 1) as zona2, 
			(select zona from veiculos_passagem where veiculo = vp.veiculo + 2) as zona3 
			from 
				veiculos_passagem vp) as sq where sq.zona <> sq.zona2 and sq.zona <> sq.zona3");		
		$limites = array();
		while($res = pg_fetch_array($rs)){
			$limites[$res["veiculo_fim"]] = 1;
		}
		
		$rodada = 1;
		$local = 1;
		$rs = pg_query($conn, "select * from veiculos_passagem");
		$quant_veiculos = 0;
		
		$controle_veiculos = array();
		$zona_veiculo = array();
		
		
		while($res = pg_fetch_array($rs)){
			echo $res["veiculo"].' c: '.$res["classificacao"].' - '.$res["zona"].' - r: '.$rodada.'<br />';
			
			if(@$limites[$res["veiculo"]]==1) $local++;			
			$quant_veiculos++;
			
			$srodada = $rodada;
			$szona = $res["zona"];
			$sclassificacao = $res["classificacao"];
			if(@(int)$zona_veiculo[$srodada][$szona][$sclassificacao] == 0){
				$zona_veiculo[$srodada][$szona][$sclassificacao] = 1;
			}else $zona_veiculo[$srodada][$szona][$sclassificacao] += 1;
			
			if($local%4==0){
				$local = 1;
				$rodada++;
				echo "> Total $quant_veiculos veiculos <br />";
				echo "----------------------RODADA $rodada----------------------------------- <br />";
				$quant_veiculos = 0;
			}
		}
		
		//cálculo do modelo
		$todos_veiculos_rodada = 0;
		$zonas_computadas = 0;
		foreach($zona_veiculo as $xrd => $rodada){
			echo "<br />----------------- inicio processamento da rodada $xrd ----------------- <br />";
			$contador_total = array();			
			$contador_tipo = array();			
			foreach($rodada as $xxd => $zona){
				$zonas_computadas++;
				if(@$contador_total[$xxd] == "") $contador_total[$xxd] = 0;
				foreach($zona as $xnd => $classificacao){
					$contador_total[$xxd] += $classificacao;
					if(@$contador_tipo[$xxd][$xnd] == "") $contador_tipo[$xxd][$xnd] = 0;
					$contador_tipo[$xxd][$xnd] += $classificacao;
					$todos_veiculos_rodada += $classificacao;
					//echo $classificacao.'-'.$xnd.'<br />';
				}				
			}
			
			/*$zona_tempo["1"] = 15;
	$zona_tempo["2"] = 30;
	$zona_tempo["3"] = 15;
	
	$tempo_total_rodadas = 0;*/
			
			
			
			//processar zonas - a fim de 
			$porcentagens = array();
			$porcentagens_tipo = array();
			
			foreach($contador_total as $ct => $vct){
				$porcentagem_todos = round($vct / $todos_veiculos_rodada * 100);
				@$quant_leves = $contador_tipo[$ct]["1"];
				$quant_pesados = $vct - $quant_leves;
				$lvpes = $quant_leves + $quant_pesados;
				$porcentagem_leves = round($quant_leves / $lvpes * 100);
				$porcentagem_pesados = round($quant_pesados / $lvpes * 100);
				
				$porcentagens[$ct] = $porcentagem_todos;
				$porcentagens_tipo[$ct]["leves"] = $porcentagem_leves;
				$porcentagens_tipo[$ct]["pesados"] = $porcentagem_pesados;
				
				$porcentagem_atual = round($zona_tempo[$ct] / $tempo_total_rodadas * 100);
				echo ">>/zona atual = $porcentagem_atual%/ zona $ct ($vct)[$todos_veiculos_rodada] - rodada $xrd = $porcentagem_todos% ////// leves $porcentagem_leves% ///////// pesados $porcentagem_pesados%<br />";				
			}
			
			$zona_a_ganhar = 0;
			$maior_valor = -99999999;
			$quantidade_a_ganhar = 0;
			if($zonas_computadas == $quant_zonas_rodadas){
				foreach($porcentagens as $d => $v){
					@$leves = (int)$porcentagens_tipo[$d]["leves"];
					@$pesados = (int)$porcentagens_tipo[$d]["pesados"];				
					if($v <= 65){ // se não tiver 65% do tempo da via
						if($v > $maior_valor){
							$maior_valor = $v;
							$zona_a_ganhar = $d;
							$diff = 100 - ($zona_tempo[$d] / $tempo_total_rodadas * 100);
							$quantidade_a_ganhar = ($diff / 100) * ($tempo_total_rodadas / $quant_zonas_rodadas) * ($v/100/$quant_zonas_rodadas);
							
						}
					}
				}
				
				//incrementar na zona que ganha e peder na zona que perde
				echo "zona a ganhar $zona_a_ganhar = $quantidade_a_ganhar<br />";
				foreach($zona_tempo as $d => $v){
					if($d == $zona_a_ganhar){
						$zona_tempo[$d] += $quantidade_a_ganhar;
					}else{
						$zona_tempo[$d] -= ($quantidade_a_ganhar / ($quant_zonas_rodadas - 1));
					}
				}
				
			}
			//zerar as variáveis
			$todos_veiculos_rodada = 0;
			$zonas_computadas = 0;
			echo "<br />----------------- fim processamento da rodada $xrd ----------------- <br />";
		}
		
	}catch(Exception $e){
		echo $e->getMessage();
	}
	//echo "fim";
	//print_r($zona_veiculo);
?>
