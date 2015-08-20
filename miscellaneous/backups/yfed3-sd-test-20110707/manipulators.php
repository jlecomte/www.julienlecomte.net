<?php

class GSPResponseInfo_OK extends GSPResponseInfo
{
    public function __construct() {}

    public $ok = true;

    public function ok()
    {
        return $this->ok;
    }
}

class URSA_Dispatcher extends YFedWorkflowDispatcher
{
    public function configure($dataModel)
    {
        $dispatcher = new GSPDispatcher;
        $provider = new GSPProviderURSA;

        $configurator = new Web_ConfiguratorURSA;
        $configurator->visit($provider, $dispatcher, $dataModel);

        return array(
            'url' => $provider->getUri()
        );
    }

    public function extract($dataModel, $data)
    {
        // Note: in order to use the old extractors, we'd have to create a new
        // dispatch layer class (GSPDispatchLayer_YFed3?) Instead, we just
        // cheat for the sake of prototyping...
        $response = new GSPResponseInfo_OK;
        $meta = new GSPResultMeta_Set(0, 0, 1);
        $content = (object) json_decode($data);
        $dataModel->yfedResponse['URSA'] = new GSPResult($response, $meta, $content);
    }
}

