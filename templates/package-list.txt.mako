<%!
    import config.deps
%># THIS IS AN AUTO-GENERATED FILE. DO NOT EDIT!
% if hasattr(config.deps, "packages"):
% for a in config.deps.packages:
${a}
% endfor
% endif
