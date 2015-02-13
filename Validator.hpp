#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

class EditorMainWindow;

class Validator {

public:

    Validator ();

    /**
     * Validates the scenario to see if it is valid and can be played. A valid scenario needs units, headquarters etc.
     *
     * @param editor
     *
     * @return true if the scenario is valid and false if not.
     */
    bool validate (EditorMainWindow * editor);


private:

    bool validateMetaData (EditorMainWindow * editor);

    bool validateUnits (EditorMainWindow * editor);

    bool validateObjectives (EditorMainWindow * editor);
};

#endif // VALIDATOR_HPP
