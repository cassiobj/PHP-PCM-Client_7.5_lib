<?php 

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PHP-PCM-Client-lib
  PHP Extension interact with Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

$totErros = 0;
$totSucessos = 0;
$totTeste=0;


$custom_fld = "C_FLD_CHARGE_PENALTY_EXEMPT_FLAG";
$custom_opcode = "C_OP_COLLECTIONS_POL_EXIT_CREDIT_INSTALLMENT";

// put here a valid invoice to read 
$flist_invoice = "0 PIN_FLD_POID       POID [0] 0.0.0.1 /invoice 226060 0";

$update_register = "0 PIN_FLD_POID       POID [0] 0.0.0.1 /account 159593 99
0 PIN_FLD_EFFECTIVE_T      TSTAMP [0] (1751515436))";

echo "|Starting Test PCM-PHP Functions          |Status| Note
|                                         |      |\n";



// test change context
echo "|Testing CM Connection                    |";
if ( pcm_set_context(1, 0) == 0)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";


// test reading
echo "|Testing Reading                          |";
if ( ($xml_invoice = pcm_op(PCM_OP_READ_OBJ, $flist_invoice, PCM_OPFLG_NO_DESCEND)) != NULL)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";
echo $xml_invoice;
echo "";

// test change context
echo "|Testing Context Operations               |";
if ( pcm_set_context(1, 0) == 0)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";

// test Transaction Open
echo "|Testing Open Transaction                 |";
if ( pcm_open_transaction(PCM_TRANS_OPEN_READWRITE) == 0)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";

// updatign register
echo "|Updating Register for Rollback           |";
if ( ($xml_str = pcm_op(PCM_OP_WRITE_FLDS, $update_register, 0)) != NULL)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";


// test Transaction abort
echo "|Testing Abort Transaction                |";
if ( pcm_abort_transaction() == 0)
{
    echo "OK    |";

     
    if ( ( $xml_str = pcm_op(PCM_OP_READ_FLDS, $update_register, 0) ) != NULL)
    {

        $dom = new DOMDocument();
        $dom->loadXML($xml_str);

    
        $pcmClient = $dom->getElementsByTagName('PCM_CLIENT')->item(0);


        if ($pcmClient->hasChildNodes()) {
            // Obter a lista de todos os filhos
            $children = $pcmClient->childNodes;
            // Encontrar o último nó de elemento
            $lastElementNode = null;
            foreach ($children as $child) {
                if ($child->nodeType === XML_ELEMENT_NODE) {
                    $lastElementNode = $child;
                }
            }

            // Se o último nó de elemento foi encontrado, imprimir o nome e valor
            if ($lastElementNode !== null) {
                $campo = " " . $lastElementNode->nodeName . ' after rollback = ' . $lastElementNode->nodeValue ;
            } else {
                $campo = "Field not Found";
            }
        } 
        else 
        {
            $campo = "Field not Found";
        }
        echo $campo;    
    }
    else
    {
        echo     "Error Searching updated field";    
    }
}
else
    echo     "Error |";

echo "\n";


// test change context
echo "|Testing Context Operations Return        |";
if ( pcm_set_context(0) == 0)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";



// test Transaction Open
echo "|Testing Open New Transaction             |";
if ( pcm_open_transaction(PCM_TRANS_OPEN_READWRITE) == 0)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";

// updatign register
echo "|Updating Register for Rollback           |";
if ( ($xml_update = pcm_op(PCM_OP_WRITE_FLDS, $update_register, 0)) != NULL)
{
    echo "OK    |";
}
else
    echo     "Error |";

echo "\n";


// test Transaction abort
echo "|Testing Commit Transaction               |";
if ( pcm_commit_transaction() == 0)
{
    echo "OK    |";

     
    if ( ( $xml_str = pcm_op(PCM_OP_READ_FLDS, $update_register, 0) ) != NULL)
    {

        $dom = new DOMDocument();
        $dom->loadXML($xml_str);

    
        $pcmClient = $dom->getElementsByTagName('PCM_CLIENT')->item(0);


        if ($pcmClient->hasChildNodes()) {
            // Obter a lista de todos os filhos
            $children = $pcmClient->childNodes;
            // Encontrar o último nó de elemento
            $lastElementNode = null;
            foreach ($children as $child) {
                if ($child->nodeType === XML_ELEMENT_NODE) {
                    $lastElementNode = $child;
                }
            }

            // Se o último nó de elemento foi encontrado, imprimir o nome e valor
            if ($lastElementNode !== null) {
                $campo = " " . $lastElementNode->nodeName . ' after commit = ' . $lastElementNode->nodeValue ;
            } else {
                $campo = "Field not Found";
            }
        } 
        else 
        {
            $campo = "Field not Found";
        }
        echo $campo;    
    }
    else
    {
        echo     "Error Searching updated field";    
    }
}
else
    echo     "Error |";

echo "\n";


/*
$id = pcm_get_poid_number("0.0.0.1 /balance_group 158441 0");
echo "\n";
$type =  pcm_get_poid_type("0.0.0.1 /balance_group 158441 0");
echo "\n";
$db = pcm_get_poid_db("0.0.0.1 /balance_group 158441 0");

echo pcm_create_poid($db, $type, $id);
*/

/*
print_r(pcm_get_field_spec("C_FLD_CHARGE_PENALTY_EXEMPT_FLAG"));
*/

// test reading



$doc = new DOMDocument();
$doc->loadXML($xml_update);


$valid_poid = trim($doc->getElementsByTagName('PIN_FLD_POID')->item(0)->nodeValue);
echo "|Testing Functions------------------------|------|";
echo " POID: " . $valid_poid . "\n";
echo "|Getting Poid DB                          |";
if ( ( $db = pcm_get_poid_db($valid_poid)  ) != NULL)
{
    echo "OK    | DB: " . $db;
}
else
    echo     "Error |";

echo "\n";


echo "|Getting Poid Type                        |";
if ( ( $type = pcm_get_poid_type($valid_poid)  ) != NULL)
{
    echo "OK    | TYPE: " . $type;
}
else
    echo     "Error |";

echo "\n";

echo "|Getting Poid Number                      |";
if ( ( $id = pcm_get_poid_number($valid_poid)  ) != NULL)
{
    echo "OK    | ID: " . $id;
}
else
    echo     "Error |";

echo "\n";



echo "|Getting Field Spec,                      |";
if ( ( $field = pcm_get_field_spec("PIN_FLD_DUE_T") ) != NULL)
{
    echo "OK    | TYPE: " . $field["type"];
}
else
    echo     "Error |";

echo "\n";



echo "|Getting Custom Field Spec,               |";
if ( ( $field = pcm_get_field_spec($custom_fld) ) != NULL)
{
    echo "OK    | TYPE: " . $field["type"];
}
else
    echo     "Error |";

echo "\n";



echo "|Getting Custom Opcode Number             |";
if ( ( $number = pcm_get_custom_opcode($custom_opcode) ) > 0)
{
    echo "OK    | NUMBER: " . $number;
}
else
    echo     "Error |";

echo "\n";


//$xml_invoice = "";

echo "|Testing XML to Flist                     |";
if ( ( $flist = pcm_xml_to_flist($xml_invoice) ) != NULL)
{       
    if (substr_count($flist, "\n") > 0 && (strpos($flist, "PHP Warning") == false)  )    
        echo "OK    |";    
    else
        echo     "Error |";
}
else
    echo     "Error |";

echo "\n";


echo "|pin_virtual_time                         |";
if (  pin_virtual_time()  > 0)
{
    echo "OK    | DATE: " . date('d/m/Y h:i:s a ', pin_virtual_time());
}
else
    echo     "Error |";

echo "\n";





?>