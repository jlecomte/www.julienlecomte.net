<?php

// -- Backward compatibility with existing GSP2 manipulators and factory ------

class YFedWorkflowFactory extends GSPDataModelFactory
{
    protected $lastRegisteredComponent = null;

    public function __construct(GSPDataModel $dataModel)
    {
        parent::__construct($dataModel);
        yahoo_yfed3_initialize_session();
    }

    public function __destruct()
    {
        yahoo_yfed3_release_session();
    }

    public function register($name, GSPDataManipulator $manipulator, Array $dependencies = null)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));

        if ($this->isRegistered($name)) {
            return false;
        }

        $last = $this->lastRegisteredComponent;

        if (empty($dependencies) && $last) {
            $dependencies = array($last);
        }

        if (empty($dependencies) || $last && in_array($last, $dependencies)) {
            $this->lastRegisteredComponent = $name;
        }

        $adapter = new YFedWorkflowManipulatorAdapter($name, $manipulator, $this, $dependencies);
        return yahoo_yfed3_register_component($adapter);
    }

    public function registerBefore($name, $newName, GSPDataManipulator $manipulator)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));
        #debug "gsp_assert" assert(is_string($newName) && !empty($newName));

        $component = yahoo_yfed3_get_component($name);
        if (!$component) {
            return false;
        }

        $retval = $this->register($newName, $manipulator, $component->dependencies);

        // Update the existing component's dependencies,
        // and use $newName as its single dependency.
        if ($retval) {
            $component->dependencies = array($newName);
        }

        return $retval;
    }

    public function registerAfter($name, $newName, GSPDataManipulator $manipulator)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));
        #debug "gsp_assert" assert(is_string($newName) && !empty($newName));

        if (!$this->isRegistered($name)) {
            return false;
        }

        // Find all components that depend on $name and change that to $newName.
        $list = yahoo_yfed3_enum_components();

        foreach ($list as $component) {
            $dependencies = &$component->dependencies;
            if (!empty($dependencies)) {
                foreach ($dependencies as $index => $dependency) {
                    if ($dependency === $name) {
                        $dependencies[$index] = $newName;
                    }
                }
            }
        }

        // Finally, register the new component, using $name as its single dependency.
        return $this->register($newName, $manipulator, array($name));
    }

    public function unregister($name, $instance = false)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));

        return yahoo_yfed3_unregister_component($name);
    }

    public function reRegister($name, GSPDataManipulator $manipulator, Array $dependencies = null)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));

        if (!$this->isRegistered($name)) {
            return false;
        }

        return true;
    }

    public function registerDispatcher($name, YFedWorkflowDispatcher $dispatcher, Array $dependencies = null)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));

        if ($this->isRegistered($name)) {
            return false;
        }

        $last = $this->lastRegisteredComponent;

        if (empty($dependencies) && $last) {
            $dependencies = array($last);
        }

        if (empty($dependencies) || $last && in_array($last, $dependencies)) {
            $this->lastRegisteredComponent = $name;
        }

        // When using this method, the name and dependencies declared in the
        // dispatcher class itself are overriden. Is this a good thing?
        // I'm not sure! I did this so that the factory interface is similar
        // for manipulators and dispatchers...
        $dispatcher->name = $name;
        $dispatcher->dependencies = $dependencies;

        return yahoo_yfed3_register_component($dispatcher);
    }

    public function run()
    {
        return yahoo_yfed3_execute($this->_dataModel);
    }

    public function enum()
    {
        $manipulators = array();

        $list = yahoo_yfed3_enum_components();
        foreach ($list as $name => $component) {
            if ($component instanceof YFedWorkflowManipulatorAdapter) {
                $manipulators[$name] = $component->manipulator;
            }
        }

        return array_keys($manipulators);
    }

    public function isRegistered($name)
    {
        #debug "gsp_assert" assert(is_string($name));

        $component = yahoo_yfed3_get_component($name);
        return ($component !== null);
    }

    public function getManipulator($name)
    {
        #debug "gsp_assert" assert(is_string($name));

        $component = yahoo_yfed3_get_component($name);
        if ($component !== null && $component instanceof YFedWorkflowManipulatorAdapter) {
            return $component->manipulator;
        } else {
            return null;
        }
    }

    public function getDispatcher($name)
    {
        #debug "gsp_assert" assert(is_string($name));

        $component = yahoo_yfed3_get_component($name);
        if ($component !== null && $component instanceof YFedWorkflowDispatcherAdapter) {
            return $component->dispatcher;
        } else {
            return null;
        }
    }

    public function clearWeightsAfter($name, $through = false)
    {
        #debug "gsp_log_warn" GSPLog::warn("Method clearWeightsAfter is not implemented in class YFedWorkflowFactory");

        return false;
    }

    public function runTableCount($name)
    {
        #debug "gsp_assert" assert(is_string($name));

        return $this->isRegistered($name) ? 1 : 0;
    }

    public function prettyPrintNode($dh, $parent)
    {
        return ''; // TODO
    }

    public function dumpRunTable()
    {
        return ''; // TODO
    }
}

class YFedWorkflowManipulatorAdapter extends YFedWorkflowManipulator
{
    public $manipulator = null;
    protected $factory = null;

    public function __construct($name, GSPDataManipulator $manipulator,
        YFedWorkflowFactory $factory, Array $dependencies = null)
    {
        #debug "gsp_assert" assert(is_string($name) && !empty($name));

        $this->name = $name;
        $this->manipulator = $manipulator;
        $this->factory = $factory;
        $this->dependencies = $dependencies;
    }

    public function run($dataModel)
    {
        // TODO: should we look at the return value?
        $this->manipulator->run($this->name, $this->factory, $dataModel);
    }
}
