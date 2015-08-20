<?php

error_reporting(E_ERROR);

class GSPDataModel {}
class TestDataModel extends GSPDataModel {}

class FakeManipulator
{
    public $name = '';
    public $dependencies = null;
    public $hasRun = false;

    public function run($dataModel) {}
}

class TestWorkflowManipulator extends YFedWorkflowManipulator
{
    public function run($dataModel)
    {
        echo "RUN_{$this->name}\n";
    }
}

class TestWorkflowDispatcher extends YFedWorkflowDispatcher
{
    public function configure($dataModel)
    {
        echo "CONFIGURE_{$this->name}\n";
    }

    public function extract($dataModel, $data)
    {
        echo "EXTRACT_{$this->name}\n";
    }
}

class A extends TestWorkflowManipulator
{
    public $name = 'A';
}

class B extends TestWorkflowManipulator
{
    public $name = 'B';

    public function run($dataModel)
    {
        parent::run($dataModel);
        // Try to dynamically add a component, while the workflow is executing...
        assert(yahoo_yfed3_register_component(new F) === true);
    }
}

class C extends TestWorkflowDispatcher
{
    public $name = 'C';
    public $dependencies = array('A', 'B');

    public function configure($dataModel)
    {
        parent::configure($dataModel);

        return array(
            'url' => 'http://sports.yahoo.com/'
        );
    }
}

class D extends TestWorkflowDispatcher
{
    public $name = 'D';
    public $dependencies = array('A');

    public function configure($dataModel)
    {
        parent::configure($dataModel);

        return array(
            'url' => 'http://news.yahoo.com/'
        );
    }
}

class E extends TestWorkflowManipulator
{
    public $name = 'E';
    public $dependencies = array('B', 'D');
}

class F extends TestWorkflowManipulator
{
    public $name = 'F';
    public $dependencies = array('B', 'C', 'E');
}

class G extends TestWorkflowManipulator
{
    public $name = 'G';
    public $dependencies = array('H');
}

class H extends TestWorkflowManipulator
{
    public $name = 'H';
    public $dependencies = array('G');
}

$dataModel = new TestDataModel;

function my_assert_handler($file, $line, $code)
{
    echo "Assertion Failed (line $line)\n";
    exit;
}

assert_options(ASSERT_CALLBACK, 'my_assert_handler');

// -- Start test --------------------------------------------------------------

// Try calling API functions before initialization.
assert(yahoo_yfed3_register_component(new A) === false);
assert(yahoo_yfed3_unregister_component('A') === false);
assert(yahoo_yfed3_get_component('A') === false);
assert(yahoo_yfed3_enum_components() === null);
assert(yahoo_yfed3_execute($dataModel) === false);
assert(yahoo_yfed3_release_session() === true);

// Try multiple initializations.
assert(yahoo_yfed3_initialize_session() === true);
assert(yahoo_yfed3_initialize_session() === true);
assert(yahoo_yfed3_initialize_session() === true);

// Exercise the API.
assert(yahoo_yfed3_register_component(new A) === true);
assert(yahoo_yfed3_register_component(new A) === false);
assert(yahoo_yfed3_unregister_component('A') === true);
assert(yahoo_yfed3_unregister_component('B') === false);
assert(yahoo_yfed3_register_component(new FakeManipulator) === false);
assert(yahoo_yfed3_enum_components() == array());

// Try to run a simple workflow.
assert(yahoo_yfed3_register_component(new A) === true);
assert(yahoo_yfed3_register_component(new B) === true);
assert(yahoo_yfed3_register_component(new C) === true);
assert(yahoo_yfed3_register_component(new D) === true);
assert(yahoo_yfed3_register_component(new E) === true);

// Exercise yahoo_yfed3_enum_components...
$components = yahoo_yfed3_enum_components();
$component_names = implode('|', array_keys($components));
assert($component_names === 'A|B|C|D|E');

ob_start();
assert(yahoo_yfed3_execute($dataModel) === true);
$output = trim(ob_get_contents());
ob_end_clean();

$lines = explode("\n", $output);
$output = implode('|', $lines);

assert($output === 'RUN_A|CONFIGURE_D|RUN_B|CONFIGURE_C|EXTRACT_C|EXTRACT_D|RUN_E|RUN_F' ||
       $output === 'RUN_A|CONFIGURE_D|RUN_B|CONFIGURE_C|EXTRACT_D|RUN_E|EXTRACT_C|RUN_F' ||
       $output === 'RUN_A|CONFIGURE_D|RUN_B|CONFIGURE_C|EXTRACT_D|EXTRACT_C|RUN_E|RUN_F');

// Try circular dependencies...
assert(yahoo_yfed3_register_component(new G) === true);
assert(yahoo_yfed3_register_component(new H) === true);
assert(count(yahoo_yfed3_enum_components()) === 8);
assert(yahoo_yfed3_execute() === false);

// Clean things up, multiple times :)
assert(yahoo_yfed3_release_session() === true);
assert(yahoo_yfed3_release_session() === true);
assert(yahoo_yfed3_release_session() === true);

echo "All tests passed successfully.\n";
