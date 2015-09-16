select 
	sum(subquery.difftemp) / count(subquery.difftemp) as media_intervalo_veiculos
from 
	(select (select tempo from veiculos_passagem where veiculo = vp.veiculo + 1) - vp.tempo as difftemp from veiculos_passagem vp) as subquery;

select 
((select tempo from veiculos_passagem where veiculo = (select max(veiculo) from veiculos_passagem)) - 
(select tempo from veiculos_passagem where veiculo = (select max(veiculo) - 1 from veiculos_passagem))) as ultimo_tempo_passagem

select count(*) from veiculos_passagem where classificacao = 0 and zona = 1
select count(*) from veiculos_passagem where classificacao = 1 and zona = 1
