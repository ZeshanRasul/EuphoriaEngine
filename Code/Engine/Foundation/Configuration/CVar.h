#pragma once

#include <Foundation/Communication/Event.h>
#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Types/Bitflags.h>
#include <Foundation/Utilities/EnumerableClass.h>

class plCVar;

/// \brief Describes of which type a CVar is. Use that info to cast an plCVar* to the proper derived class.
struct plCVarType
{
  enum Enum
  {
    Int,    ///< Can cast the plCVar* to plCVarInt*
    Float,  ///< Can cast the plCVar* to plCVarFloat*
    Bool,   ///< Can cast the plCVar* to plCVarBool*
    String, ///< Can cast the plCVar* to plCVarString*
    ENUM_COUNT
  };
};

/// \brief The flags that can be used on an plCVar.
struct plCVarFlags
{
  using StorageType = plUInt8;

  enum Enum
  {
    None = 0,

    /// \brief If this flag is set, the CVar will be stored on disk and loaded again.
    /// Otherwise all changes to it will be lost on shutdown.
    Save = PL_BIT(0),

    /// \brief If the CVar value is changed, the new value will not be visible by default, until SetToDelayedSyncValue() is called on it.
    /// This allows to finalize the value change at a specific sync point in code.
    /// When this flag is set the plCVarEvent::DelayedSyncValueChanged will be broadcast.
    RequiresDelayedSync = PL_BIT(1),

    ShowRequiresRestartMsg = PL_BIT(2),

    /// \brief Indicates that changing this CVar will only take effect after the proper subsystem has been reinitialized.
    /// This will always enforce the 'Save' flag as well.
    /// With this flag set, the 'Current' value never changes, unless 'SetToDelayedSyncValue' is called.
    RequiresRestart = Save | RequiresDelayedSync | ShowRequiresRestartMsg,

    /// \brief By default CVars are not saved.
    Default = None
  };

  struct Bits
  {
    StorageType Save : 1;
    StorageType RequiresDelayedSync : 1;
    StorageType ShowRequiresRestartMsg : 1;
  };
};

PL_DECLARE_FLAGS_OPERATORS(plCVarFlags);

/// \brief The data that is broadcast whenever a cvar is changed.
struct plCVarEvent
{
  plCVarEvent(plCVar* pCVar)
    : m_pCVar(pCVar)
  {
  }

  enum Type
  {
    ValueChanged,            ///< Sent whenever the 'Current' value of the CVar is changed.
    DelayedSyncValueChanged, ///< Sent whenever the 'DelayedSync' value of the CVar changes. It might actually change back to the 'Current' value though.
    ListOfVarsChanged,       ///< A CVar was added or removed dynamically (not just by loading a plugin), some stuff may need to update its state
  };

  /// \brief The type of this event.
  Type m_EventType = ValueChanged;

  /// \brief Which CVar is involved. This is only for convenience, it is always the CVar on which the event is triggered.
  plCVar* m_pCVar;
};

/// \brief CVars are global variables that are used for configuring the engine.
///
/// The state of a CVar can be automatically stored when the application is shut down, and during reloading of plugins.
/// It will be restored again when the application starts again.
/// This makes it possible to use them to tweak code that is work in progress or to change global settings.
/// CVars are enumerable, which is why it is easy to present them in a console or a GUI at runtime, to allow modifying them
/// while the application is running.
/// It is very easy and convenient to temporarily add a CVar while some code is in development, to be able to try out different
/// approaches. However, one should throw out all unnecessary variables after such work is finished.
/// CVars are stored in one settings file per plugin. That means plugins can easily contain additional CVars for their own use
/// and their states are restored at plugin loading time, as well.
/// For the storage of CVars to work, the 'StorageFolder' must have been set. Also at startup the application should explicitly
/// load CVars via 'LoadCVars', once the filesystem is set up and the storage folder is configured.
/// The CVar system listens to events from the Plugin system, and it will automatically take care to serialize and deserialize
/// CVar values whenever plugins are loaded or unloaded.
/// CVars additionally allow to only change their visible value after a certain subsystem has been 'restarted', i.e. a user can
/// change the CVar value at runtime, but when the 'current' value is read, it will not have changed.
/// It will change however, once the application is restarted (such that code can initialize the engine with the correct values)
/// or after the corresponding subsystem explicitly sets the CVar to the updated value.
/// This is useful, e.g. for a screen resolution CVar, as changing this at runtime might be possible in a GUI, but the engine
/// might not support that without a restart.
/// Finally all CVars broadcast events when their value is changed, which can be used to listen to certain CVars and react
/// properly when their value changes.
class PL_FOUNDATION_DLL plCVar : public plEnumerable<plCVar>
{
  PL_DECLARE_ENUMERABLE_CLASS(plCVar);

public:
  /// \brief Sets the path (folder) in which all CVar setting files should be stored.
  ///
  /// The path is used by SaveCVars and LoadCVars. However those functions will create one settings file per plugin,
  /// so \a szFolder must not be a file name, but only a path to a folder.
  ///
  /// After setting the storage folder, one should immediately load all CVars via LoadCVars.
  static void SetStorageFolder(plStringView sFolder); // [tested]

  /// \brief Searches all CVars for one with the given name. Returns nullptr if no CVar could be found. The name is case-insensitive.
  static plCVar* FindCVarByName(plStringView sName); // [tested]

  /// \brief Stores all CVar values in files in the storage folder, that must have been set via 'SetStorageFolder'.
  ///
  /// This function has no effect, if 'SetStorageFolder' has not been called, or the folder has been set to be empty.
  /// This function is also automatically called whenever plugin changes occur, or when the engine is shut down.
  /// So it might not be necessary to call this function manually at shutdown.
  static void SaveCVars(); // [tested]

  /// \brief Stores all CVar values into the given file.
  ///
  /// This function works without setting a storage folder.
  /// If bIgnoreSaveFlag is set all CVars are saved whether they have the plCVarFlags::Save set or not.
  ///
  /// \sa LoadCVarsFromFile()
  static void SaveCVarsToFile(plStringView sPath, bool bIgnoreSaveFlag = false);

  /// \brief Calls LoadCVarsFromCommandLine() and then LoadCVarsFromFile()
  static void LoadCVars(bool bOnlyNewOnes = true, bool bSetAsCurrentValue = true); // [tested]

  /// \brief Loads the CVars from the settings files in the storage folder.
  ///
  /// The CVars are loaded into the global system and thus automatically available everywhere after this call.
  /// Optionally they are returned via pOutCVars, so the caller knows which CVars have actually been
  /// loaded in this very step.
  ///
  /// This function has no effect, if the storage folder has not been set via 'SetStorageFolder' yet
  /// or it has been set to be empty.
  ///
  /// If \a bOnlyNewOnes is set, only CVars that have never been loaded from file before are loaded.
  /// All other CVars will stay unchanged.
  /// If \a bSetAsCurrentValue is true, variables that are flagged as 'RequiresRestart', will be set
  /// to the restart value immediately ('SetToDelayedSyncValue' is called on them).
  /// Otherwise their 'Current' value will always stay unchanged and the value from disk will only be
  /// stored in the 'DelayedSync' value.
  /// Independent on the parameter settings, all CVar changes during loading will always trigger change events.
  ///
  ///
  /// \sa LoadCVarsFromCommandLine()
  static void LoadCVarsFromFile(bool bOnlyNewOnes = true, bool bSetAsCurrentValue = true, plDynamicArray<plCVar*>* pOutCVars = nullptr); // [tested]

  /// \brief Loads all CVars from the given file. Does not account for any plug-in specific files.
  ///
  /// The CVars are loaded into the global system and thus automatically available everywhere after this call.
  /// Optionally they are returned via pOutCVars, so the caller knows which CVars have actually been
  /// loaded in this very step.
  ///
  /// This function works without setting a storage folder.
  ///
  /// If \a bOnlyNewOnes is set, only CVars that have never been loaded from file before are loaded.
  /// All other CVars will stay unchanged.
  /// If \a bSetAsCurrentValue is true, variables that are flagged as 'RequiresRestart', will be set
  /// to the restart value immediately ('SetToDelayedSyncValue' is called on them).
  /// Otherwise their 'Current' value will always stay unchanged and the value from disk will only be
  /// stored in the 'DelayedSync' value.
  /// Independent on the parameter settings, all CVar changes during loading will always trigger change events.
  /// If bIgnoreSaveFlag is set all CVars are loaded whether they have the plCVarFlags::Save set or not.
  ///
  /// \sa LoadCVarsFromCommandLine()
  /// \sa LoadCVarsFromFile()
  static void LoadCVarsFromFile(plStringView sPath, bool bOnlyNewOnes = true, bool bSetAsCurrentValue = true, bool bIgnoreSaveFlag = false, plDynamicArray<plCVar*>* pOutCVars = nullptr);

  /// \brief Similar to LoadCVarsFromFile() but tries to get the CVar values from the command line.
  ///
  /// The CVars are loaded into the global system and thus automatically available everywhere after this call.
  /// Optionally they are returned via pOutCVars, so the caller knows which CVars have actually been
  /// loaded in this very step.
  ///
  /// \note A CVar will only ever be loaded once. This function should be called before LoadCVarsFromFile(),
  /// otherwise it could get flagged as 'already loaded' even if the value was never taken from file or command line.
  static void LoadCVarsFromCommandLine(bool bOnlyNewOnes = true, bool bSetAsCurrentValue = true, plDynamicArray<plCVar*>* pOutCVars = nullptr); // [tested]

  /// \brief Copies the 'DelayedSync' value into the 'Current' value.
  ///
  /// This change will not trigger a 'delayed sync value changed' event, but it might trigger a 'current value changed' event.
  /// Code that uses a CVar that is flagged as 'RequiresDelayedSync' for its initialization (and which is the reason, that that CVar
  /// is flagged as such) should always call this BEFORE it uses the CVar value.
  virtual void SetToDelayedSyncValue() = 0; // [tested]

  /// \brief Returns the (display) name of the CVar.
  plStringView GetName() const { return m_sName; } // [tested]

  /// \brief Returns the type of the CVar.
  virtual plCVarType::Enum GetType() const = 0; // [tested]

  /// \brief Returns the description of the CVar.
  plStringView GetDescription() const { return m_sDescription; } // [tested]

  /// \brief Returns all the CVar flags.
  plBitflags<plCVarFlags> GetFlags() const { return m_Flags; } // [tested]

  using CVarEvents = plEvent<const plCVarEvent&, plMutex, plStaticsAllocatorWrapper>;

  /// \brief Code that needs to be execute whenever a cvar is changed can register itself here to be notified of such events.
  CVarEvents m_CVarEvents; // [tested]

  /// \brief Broadcasts changes to ANY CVar. Thus code that needs to update when any one of them changes can use this to be notified.
  static plEvent<const plCVarEvent&> s_AllCVarEvents;

  /// \brief Returns the name of the plugin which this CVar is declared in.
  plStringView GetPluginName() const { return m_sPluginName; }

  /// \brief Call this after creating or destroying CVars dynamically (not through loading plugins) to allow UIs to update their state.
  ///
  /// Broadcasts plCVarEvent::ListOfVarsChanged.
  static void ListOfCVarsChanged(plStringView sSetPluginNameTo);

protected:
  plCVar(plStringView sName, plBitflags<plCVarFlags> Flags, plStringView sDescription);

private:
  PL_MAKE_SUBSYSTEM_STARTUP_FRIEND(Foundation, CVars);

  static void AssignSubSystemPlugin(plStringView sPluginName);
  static void PluginEventHandler(const plPluginEvent& EventData);

  /// \brief Loads CVar values for the given vars from the given config file path. Returns the plCVars which have actually been loaded.
  static void LoadCVarsFromFileInternal(plStringView path, const plDynamicArray<plCVar*>& vars, bool bSetAsCurrentValue, plDynamicArray<plCVar*>* pOutCVars);

  /// \brief Stores the values of the given vars to the given config file path.
  static void SaveCVarsToFileInternal(plStringView path, const plDynamicArray<plCVar*>& vars);

  bool m_bHasNeverBeenLoaded = true; // next time 'LoadCVars' is called, its state will be changed
  plStringView m_sName;
  plStringView m_sDescription;
  plStringView m_sPluginName;
  plBitflags<plCVarFlags> m_Flags;

  static plString s_sStorageFolder;
};

/// \brief Each CVar stores several values internally. The 'Current' value is the most important one.
struct plCVarValue
{
  enum Enum
  {
    Current,     ///< The value that should be used.
    Default,     ///< The 'default' value of the CVar. Can be used to reset a variable to its default state.
    Stored,      ///< The value that was read from disk (or the default). Can be used to reset a CVar to the 'saved' state, if desired.
    DelayedSync, ///< The state that will be stored for later. This is identical to 'Current' unless the 'RequiresDelayedSync' flag is set (in which case the 'Current' value only changes when the code requests so).
    ENUM_COUNT
  };
};

/// \brief [internal] Helper class to implement plCVarInt, plCVarFlag, plCVarBool and plCVarString.
template <typename Type, plCVarType::Enum CVarType>
class plTypedCVar : public plCVar
{
public:
  plTypedCVar(plStringView sName, const Type& value, plBitflags<plCVarFlags> flags, plStringView sDescription);

  /// \brief Returns the 'current' value of the CVar. Same as 'GetValue(plCVarValue::Current)'
  operator const Type&() const; // [tested]

  /// \brief Returns the internal values of the CVar.
  const Type& GetValue(plCVarValue::Enum val = plCVarValue::Current) const; // [tested]

  /// \brief Changes the CVar's value and broadcasts the proper events.
  ///
  /// Usually the 'Current' value is changed, unless the 'RequiresDelayedSync' flag is set.
  /// In that case only the 'DelayedSync' value is modified.
  void operator=(const Type& value); // [tested]

  virtual plCVarType::Enum GetType() const override;
  virtual void SetToDelayedSyncValue() override;

  /// \brief Checks whether a new value was set and now won't be visible until SetToDelayedSyncValue() is called.
  bool HasDelayedSyncValueChanged() const
  {
    return m_Values[plCVarValue::Current] != m_Values[plCVarValue::DelayedSync];
  }

private:
  friend class plCVar;

  Type m_Values[plCVarValue::ENUM_COUNT];
};

/// \brief A CVar that stores a float value.
using plCVarFloat = plTypedCVar<float, plCVarType::Float>;

/// \brief A CVar that stores a bool value.
using plCVarBool = plTypedCVar<bool, plCVarType::Bool>;

/// \brief A CVar that stores an int value.
using plCVarInt = plTypedCVar<int, plCVarType::Int>;

/// \brief A CVar that stores a string.
using plCVarString = plTypedCVar<plHybridString<32>, plCVarType::String>;



#include <Foundation/Configuration/Implementation/CVar_inl.h>