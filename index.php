<?php 
error_reporting(E_ALL);
ini_set('display_errors', 1);
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
		while($res = pg_fetch_array($rs)){
			echo $res["veiculo"].' - '.$res["zona"].' - r: '.$rodada.'<br />';
			if(@$limites[$res["veiculo"]]==1) $local++;
			$quant_veiculos++;
			if($local%4==0){
				$local = 1;
				$rodada++;
				echo "> Total $quant_veiculos veiculos <br />";
				echo "----------------------RODADA $rodada----------------------------------- <br />";
				$quant_veiculos = 0;
			}
		}
	}catch(Exception $e){
		echo $e->getMessage();
	}
	echo "fim";
?>
